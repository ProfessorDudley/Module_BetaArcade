#include "HP_CH_Player.h"

#if WITH_EDITOR
#include "Engine.h"
#include "Editor.h"
#endif

#include "HP_GM_LocalMultiplayer.h"
#include "HP_AI_CharAnimInstance.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayer, Log, All);

FEffectData::FEffectData(const FEffectBase& Other)
{
  Type = Other.Type;
  Duration = Other.Duration;
  Factor = Other.Factor;
  WhoAppliedEffect = Other.WhoAppliedEffect;
}

FEffectBase::FEffectBase(const FEffectBase& Other)
{
  Type = Other.Type;
  Duration = Other.Duration;
  Factor = Other.Factor;
  WhoAppliedEffect = Other.WhoAppliedEffect;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

AHP_CH_Player::AHP_CH_Player() 
{
  PrimaryActorTick.bCanEverTick = true;

  PunchShape = CreateDefaultSubobject<UBoxComponent>(TEXT("PunchCollider"));
  PunchShape->SetupAttachment(GetRootComponent());

  DashBackCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dash"));
  DashBackCollision->SetupAttachment(GetRootComponent());
  DashBackCollision->bHiddenInGame = true;

  PunchCD = CreateDefaultSubobject<UWidgetComponent>(TEXT("Punch cd bar"));
  PunchCD->SetupAttachment(GetRootComponent());

  DashCD = CreateDefaultSubobject<UWidgetComponent>(TEXT("Dash cd bar"));
  DashCD->SetupAttachment(GetRootComponent());
  

  DashData.Effect.WhoAppliedEffect = this;
  PunchData.Effect.WhoAppliedEffect = this;
  PunchData.Effect.Type = EEffect::E_KnockBack;
  PunchData.Effect.Factor = 2000.0f;
  InTugOfWar = false;
}

void AHP_CH_Player::SetActiveTriggerCollision(FSkillBase& skill, bool enable)
{

  PunchShape->SetActive(enable);

  if (enable)
  {
    PunchShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PunchShape->ClearMoveIgnoreActors();
    PunchShape->IgnoreActorWhenMoving(this, true);

    FTimerDelegate callback;
    callback.BindUFunction(this, FName("SetActiveTriggerCollision"), skill, false);

    GetWorldTimerManager().SetTimer(skill.TriggerLiveHandle, callback, 0.01f, false, skill.LiveSpanTriggerCollision);

    FComponentQueryParams params;
    params.AddIgnoredActor(this);

    TArray<FOverlapResult> overlappedObjects;
    GetWorld()->ComponentOverlapMultiByChannel(overlappedObjects,
                                               PunchShape,
                                               PunchShape->GetComponentLocation(),
                                               PunchShape->GetComponentRotation(),
                                               DashData.CollisionChannel,
                                               params);

    for (auto OverlapObject : overlappedObjects) {
      //TODO Put a proper collision channel
      if (OverlapObject.Actor != nullptr) {
        if (OverlapObject.Actor->IsA(AHP_CH_Player::StaticClass())) {
          AHP_CH_Player* player = Cast<AHP_CH_Player>(OverlapObject.Actor);

          OnPunchBeginOverlap(PunchShape, OverlapObject.Actor.Get(), OverlapObject.Component.Get(), OverlapObject.ItemIndex, false, FHitResult());
        }
      }
    }
  }
  else
  {
    PunchShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetWorldTimerManager().PauseTimer(skill.TriggerLiveHandle);
  }
}

void AHP_CH_Player::ApplyEffect(const FEffectData Data)
{
  switch (Data.Type)
  {
  case EEffect::E_None:
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is none"), *GetFName().ToString ());
    break;

  case EEffect::E_Slow:
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is slow"), *GetFName().ToString());

    GetCharacterMovement()->MaxWalkSpeed *= (1.0f - Data.Factor);

    SlowDelegate.Unbind();
    SlowDelegate.BindUFunction(this, FName("RemoveEffect"), Data);
    GetWorldTimerManager().SetTimer(SlowHandle, SlowDelegate, 0.5f, false, Data.Duration);
    break;

  case EEffect::E_KnockBack:
  {
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is knock back"), *GetFName().ToString());

    FVector director = GetActorLocation() - Data.WhoAppliedEffect->GetActorLocation();
    director.Normalize();

    LaunchCharacter(director * Data.Factor, false, false);
  }
    break;

  case EEffect::E_KnockUp:
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is knock up"), *GetFName().ToString());

    if (!GetCharacterMovement()->IsFalling())
    {
      if(Data.bActivateAfterEffectsAtTheEnd)
       LandedDelegate.AddDynamic(this, &AHP_CH_Player::OnLandAfterEffectApplied);
      LaunchCharacter(GetRootComponent()->GetUpVector() * Data.Factor, false, false);
    }

    break;
  case EEffect::E_Stun:
  {
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is stun"), *GetFName().ToString());

    APlayerController * controller = Cast<APlayerController>(GetController());

    controller->DisableInput(controller);

    StunDelegate.Unbind();
    StunDelegate.BindUFunction(this, FName("RemoveEffect"), Data);
    GetWorldTimerManager().SetTimer(StunnedHandle, StunDelegate, 0.5f, false, Data.Duration);
  }
  break;

  default:
    break;
  }

  if (!Data.bActivateAfterEffectsAtTheEnd)
  {
    //TODO Move this to a function
    for (FEffectBase effect : Data.EffectsAppliedOnEnd)
    {
      effect.WhoAppliedEffect = Data.WhoAppliedEffect;
      ApplyEffect(effect);
    }
  }
  LastEffect = Data;

}

void AHP_CH_Player::RemoveEffect(const FEffectData Data)
{
  switch (Data.Type)
  {
  case EEffect::E_None:
    UE_LOG(LogPlayer, Log, TEXT("The effect applied to %s is none"), *GetFName().ToString());
    break;

  case EEffect::E_Slow:
    GetCharacterMovement()->MaxWalkSpeed *= (1.0f + Data.Factor);
    break;

  case EEffect::E_KnockBack:
   
    break;

  case EEffect::E_KnockUp:
   
    break;
  case EEffect::E_Stun:
  {
    APlayerController* controller = Cast<APlayerController>(GetController());

    controller->EnableInput(controller);
  }
    break;

  default:
    break;
  }

}

void AHP_CH_Player::BeginPlay()
{
  Super::BeginPlay();

  SetupOverlapEvents();
  //Does not matter which skill is passed only matter deactivate it.
  SetActiveTriggerCollision(PunchData, false);

  DashBackCollision->SetActive(false);
 
}

void AHP_CH_Player::OnConstruction(const FTransform& transform)
{
  Super::OnConstruction(transform);

}

void AHP_CH_Player::PostInitializeComponents()
{
  Super::PostInitializeComponents();

}

void AHP_CH_Player::SetupOverlapEvents()
{
  PunchShape->OnComponentBeginOverlap.AddDynamic(this, &AHP_CH_Player::OnPunchBeginOverlap);
  //Added ignore actor to the punch collider
  PunchShape->IgnoreActorWhenMoving(this, true);

}

void AHP_CH_Player::OnPunchBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
  if (OtherActor->IsA(AHP_CH_Player::StaticClass()))
  {
    AHP_CH_Player* player = Cast<AHP_CH_Player>(OtherActor);

    //This is to collision only one time with a player.
    OverlappedComponent->IgnoreActorWhenMoving(OtherActor, true);

#if WITH_EDITOR
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Overlapped with player");
#endif
    if (player->CanEnterTugOfWar())
    {
      AHP_GM_LocalMultiplayer* gm = Cast<AHP_GM_LocalMultiplayer>(GetWorld()->GetAuthGameMode());
      gm->TugOfWarStartedEvent.Broadcast(player, this);
    } 
    else 
    {
      player->ApplyEffect(PunchData.Effect);
      GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "I've applied a punch effect");

      // Detaching the pickable object from the player
      if (PunchData.Effect > EEffect::E_Slow) 
      {
        FVector aux = FVector(player->GetActorLocation() - GetActorLocation());
        aux.Normalize();
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "I made him drop the mic");
        player->DropTheMicWithImpulse(aux);
      }
    }
  }
}

void AHP_CH_Player::Punch()
{
  if (CanPunch() && !IsHoldingMic() && !InTugOfWar)
  {
    LaunchCharacter(GetRootComponent()->GetForwardVector() * PunchData.Impulse, false, false);
    SetActiveTriggerCollision(PunchData, true);//Activate punch collision.

    GetWorldTimerManager().SetTimer(PunchData.CoolDownHandle, 0.5f, false, PunchData.CD);
    GetWorldTimerManager().SetTimer(PunchData.hit_in_time_handle_, 0.05f, false, PunchData.time_to_hit_);


	// PLACEHOLDER. Cri's working on this.
	UHP_AI_CharAnimInstance *anim_inst;
	anim_inst = Cast<UHP_AI_CharAnimInstance>(GetMesh()->GetAnimInstance());
  if (ensure(anim_inst)) {
    anim_inst->bIsPunching = true;
  }
	// END PLACEHOLDER
	
  }


}

void AHP_CH_Player::PickupMask()
{
  TArray<AActor*> PickupsTArray;
  GetOverlappingActors(PickupsTArray, TSubclassOf<AHP_A_Mask>());

  GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "TRY TO PICK");

  if(maskCanPickup != nullptr){
    maskCanPickup->GiveMaskToPlayer(this);
    maskCanPickup = nullptr;
  }

  /*
  for (auto* pickupOverlaped : PickupsTArray) {
    if (pickupOverlaped != nullptr) {
      if (pickupOverlaped->IsA(AHP_A_Mask::StaticClass())) {
        auto pickup = Cast<AHP_A_Mask>(pickupOverlaped);

        // If the mask is available
        if (mask->MaskName != pickup->MaskName && pickup->owner_ == nullptr) {          
          pickup->GiveMaskToPlayer(this);
        }

      }
    }
  }
  */
}

void AHP_CH_Player::Dash()
{
  if (CanDash() && !InTugOfWar)
  {
    FVector dashDirection = ProjectVectorToFloor(GetRootComponent()->GetForwardVector());

    UE_LOG(LogPlayer, Log, TEXT("Dash direction %s"), *dashDirection.ToString());

    //TODO Change this to lerping.
    LaunchCharacter(dashDirection * DashData.Impulse, false, false);

    GetWorldTimerManager().SetTimer(DashData.CoolDownHandle, 0.5f, false, DashData.CD);

    //If does not hold the mic activate the punch collision.
    if (!IsHoldingMic())
    {
      SetActiveTriggerCollision(DashData, true);//Activate punch collision.
    }
    /************************************************************************/
    /*************** Take overlapping actor when dash ***********************/
    /************************************************************************/

    FComponentQueryParams params;
    params.AddIgnoredActor(this);

    TArray<FOverlapResult> overlappedObjects;
    GetWorld()->ComponentOverlapMultiByChannel(overlappedObjects, 
                                               DashBackCollision, 
                                               DashBackCollision->GetComponentLocation(),
                                               DashBackCollision->GetComponentRotation(),
                                               DashData.CollisionChannel, params);

    for (auto OverlapObject : overlappedObjects)
    {
      //TODO Put a proper collision channel
      if (OverlapObject.Actor != nullptr)
      {
        if (OverlapObject.Actor->IsA(AHP_CH_Player::StaticClass()))
        {
          AHP_CH_Player* player = Cast<AHP_CH_Player>(OverlapObject.Actor);
          player->ApplyEffect(DashData.Effect);
        }
      }
    }
    // Animations
    UHP_AI_CharAnimInstance *anim_inst;
    anim_inst = Cast<UHP_AI_CharAnimInstance>(GetMesh()->GetAnimInstance());
    if (ensure(anim_inst)) {
      if (IsHoldingMic())
        anim_inst->bIsDashing = true;
      else
        anim_inst->bIsDiving = true;
    }
    
  }
}

float AHP_CH_Player::DashCDTimeRemaining()
{
  if (CanDash())
    return 0.0f;
  
  return GetWorldTimerManager().GetTimerRemaining(DashData.CoolDownHandle) / DashData.CD;
}

float AHP_CH_Player::PunchCDTimeRemaining()
{
  if (CanPunch())
    return 0.0f;

  return GetWorldTimerManager().GetTimerRemaining(PunchData.CoolDownHandle) / PunchData.CD;
}

void AHP_CH_Player::MoveForward(float Value)
{
  if(!InTugOfWar)
    GetCharacterMovement()->AddInputVector(FVector(Value, 0.0f, 0.0f));
}

void AHP_CH_Player::MoveRight(float Value)
{
  if(!InTugOfWar)
    GetCharacterMovement()->AddInputVector(FVector(0.0f, Value, 0.0f));
}

void AHP_CH_Player::OnLandAfterEffectApplied(const FHitResult& Hit)
{
  LandedDelegate.RemoveDynamic(this, &AHP_CH_Player::OnLandAfterEffectApplied);

  for (FEffectBase& effect : LastEffect.EffectsAppliedOnEnd)
  {
    effect.WhoAppliedEffect = LastEffect.WhoAppliedEffect;
    ApplyEffect(effect);
  }
}

void AHP_CH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
  GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString("Current Velocity:" + GetCharacterMovement()->Velocity.ToString()));
#endif
}

FVector AHP_CH_Player::ProjectVectorToFloor(const FVector& Direction)
{
  FFindFloorResult floor;

  GetCharacterMovement()->FindFloor(GetCapsuleComponent()->GetComponentLocation(), floor, false);
  if (floor.bBlockingHit)
  {
    FVector newDir = FVector::VectorPlaneProject(Direction, floor.HitResult.Normal);
    //UE_LOG(LogPlayer, Log, TEXT("Hitted floor at %s"), newDir.ToString());
    return newDir;
  }
  return FVector::ZeroVector;
}

bool AHP_CH_Player::CanDash()
{
  return !GetWorldTimerManager().IsTimerActive(DashData.CoolDownHandle);
}

bool AHP_CH_Player::CanPunch()
{
  return !GetWorldTimerManager().IsTimerActive(PunchData.CoolDownHandle);
}

bool AHP_CH_Player::IsPunchValid() {
  return GetWorldTimerManager().IsTimerActive(PunchData.hit_in_time_handle_);
}

bool AHP_CH_Player::IsSlowed()
{
  return GetWorldTimerManager().IsTimerActive(SlowHandle);
}

bool AHP_CH_Player::IsStunned()
{
  return GetWorldTimerManager().IsTimerActive(StunnedHandle);
}

void AHP_CH_Player::DropTheMic() {

  if (PickedUpObjectRef->IsValidLowLevel()) {
    PickedUpObjectRef->DetachFromPlayer();
    PickedUpObjectRef = nullptr;
  } else {
    UE_LOG(LogPlayer, Log, TEXT("The picked up object reference was null at the wrong time"));
  }
}

void AHP_CH_Player::DropTheMicWithImpulse(FVector dir) 
{
  UE_CLOG(!PickedUpObjectRef->IsValidLowLevel(), LogPlayer, Log, TEXT("The picked up object reference was null at the wrong time"));

  if (PickedUpObjectRef->IsValidLowLevel()) 
  {
    PickedUpObjectRef->DetachFromPlayerWithImpulse(dir);
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "He should have dropped the mic");
    PickedUpObjectRef = nullptr;
  } 

}


