// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_A_PickableActor.h"
#include "HP_CH_Player.h"
#include "TimerManager.h"
#include "HP_AI_CharAnimInstance.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AHP_A_PickableActor::AHP_A_PickableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  my_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
  my_mesh_->SetSimulatePhysics(true);
  RootComponent = my_mesh_;
  picked_up_ = false;
  HoldingPlayerRef = nullptr;

}

void AHP_A_PickableActor::AttachToPlayerSocket(AActor *actor_to_attach) {
  GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, "Trying To Attach Again");

  if (false == picked_up_ && CanIBeAttachedAgain()) {
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, "Attaching To Some Player");
    //AHP_CH_Player *player_ref = Cast<AHP_CH_Player>(actor_to_attach);
    HoldingPlayerRef = Cast<AHP_CH_Player>(actor_to_attach);
    if (HoldingPlayerRef != nullptr) {
      //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Character Casted Successfully");
      if (true == HoldingPlayerRef->GetMesh()->DoesSocketExist(socket_attachment_name_)) {
        //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Socket Name Found Successfully");
        my_mesh_->SetSimulatePhysics(false);
        my_mesh_->SetEnableGravity(false);
        AttachToComponent(HoldingPlayerRef->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, socket_attachment_name_);
        HoldingPlayerRef->PickedUpObjectRef = this;
        UHP_AI_CharAnimInstance *anim_inst_aux = Cast<UHP_AI_CharAnimInstance>(HoldingPlayerRef->GetMesh()->GetAnimInstance());
        anim_inst_aux->bMicPickupTrigger = true;
        picked_up_ = true;
        GetWorldTimerManager().ClearTimer(HoldingPlayerRef->TugOfWarTimerHandle);
        GetWorldTimerManager().SetTimer(HoldingPlayerRef->TugOfWarTimerHandle, 0.05f, false, HoldingPlayerRef->TimeToEnterTugOfWar);
        
      } else {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Socket Name Not Found");
      }
    } else {
      GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Character casted Badly");
    }
  }
}

void AHP_A_PickableActor::DetachFromPlayer() {
  DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
  my_mesh_->SetSimulatePhysics(true);
  my_mesh_->SetEnableGravity(true);
  HoldingPlayerRef = nullptr;
  picked_up_ = false;
  GetWorldTimerManager().SetTimer(RecatchHandler, 0.05f, false, TimeUntilCatchableAgain);
}

void AHP_A_PickableActor::DetachFromPlayerWithImpulse(FVector dir) {
  DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
  my_mesh_->SetSimulatePhysics(true);
  my_mesh_->SetEnableGravity(true);
  //ApplyRandomImpulseAtDetach();
  ApplyImpulseAtDetach(dir);
  HoldingPlayerRef = nullptr;
  picked_up_ = false;
  GetWorldTimerManager().SetTimer(RecatchHandler, 0.05f, false, TimeUntilCatchableAgain);
}

bool AHP_A_PickableActor::CanIBeAttachedAgain() {
  return !GetWorldTimerManager().IsTimerActive(RecatchHandler);
}

void AHP_A_PickableActor::ApplyRandomImpulseAtDetach() {
  int aux_rand_x = FMath::RandRange(1, 2);

  int aux_rand_y = FMath::RandRange(1, 2);
  // Getting random horizontal orientations. CL
  aux_rand_x == 2 ? aux_rand_x = -1 : aux_rand_x = 1;
  aux_rand_y == 2 ? aux_rand_y = -1 : aux_rand_y = 1;

  FVector impulse_vector = FVector(1.0f, 1.0f, 1.0f);

  // Getting random magnitude for more random impulse in X axis. CL
  int32 aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.X = ((1000 * aux_rand_x) * horizontal_magnitude_multiplier_) + 
    ((aux_magnitude_rand_x * horizontal_random_multiplier_) * aux_rand_x);

  // Getting random magnitude for more random impulse in Y axis. CL
  aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.Y = ((1000 * aux_rand_y) * horizontal_magnitude_multiplier_) +
    ((aux_magnitude_rand_x * horizontal_random_multiplier_) * aux_rand_y);

  // Getting random magnitude for more random impulse in Z axis. CL
  int32 aux_magnitude_rand_y = FMath::RandRange(7000, 13000);
  impulse_vector.Z = (7000 * vertical_magnitude_multiplier_) +
    (aux_magnitude_rand_y * vertical_random_multiplier_);

  my_mesh_->AddImpulse(impulse_vector);
}

void AHP_A_PickableActor::ApplyImpulseAtDetach(FVector dir) {

  FVector impulse_vector = FVector(1.0f, 1.0f, 1.0f);

  // Getting random magnitude for more random impulse in X axis. CL
  int32 aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.X = ((1000 * dir.X) * horizontal_magnitude_multiplier_) +
    ((aux_magnitude_rand_x * horizontal_random_multiplier_) * dir.X);

  // Getting random magnitude for more random impulse in Y axis. CL
  aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.Y = ((1000 * dir.Y) * horizontal_magnitude_multiplier_) +
    ((aux_magnitude_rand_x * horizontal_random_multiplier_) * dir.Y);

  // Getting random magnitude for more random impulse in Z axis. CL
  int32 aux_magnitude_rand_y = FMath::RandRange(7000, 13000);
  impulse_vector.Z = (7000 * vertical_magnitude_multiplier_) +
    (aux_magnitude_rand_y * vertical_random_multiplier_);

  my_mesh_->AddImpulse(impulse_vector);
}

// Called when the game starts or when spawned
void AHP_A_PickableActor::BeginPlay()
{
	Super::BeginPlay();
  //if (false == mesh_variable_->IsValidLowLevel()) {
  //  // Setting default mesh if the mesh variable is not set. CL
  //  ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Engine/BasicShapes/Cube"));
  //  my_mesh_->SetStaticMesh(MeshObj.Object);
  //}
  my_mesh_->SetStaticMesh(mesh_variable_);

}

// Called every frame
void AHP_A_PickableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

