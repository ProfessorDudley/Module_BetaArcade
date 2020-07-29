// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_A_Mask.h"

#include "HP_CH_Player.h"
#include "HP_A_Camera.h"
#include "HP_GM_LocalMultiplayer.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"

#if WITH_EDITOR
#include "Engine.h"
#include "Editor.h"
#endif

// Sets default values
AHP_A_Mask::AHP_A_Mask(const class FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer) {
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  // Root Component
  USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  RootComponent = root;

  // Set the mesh
  Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(RootComponent);

  // Create the Sphere Area
  SphereArea = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereArea"));
  SphereArea->SetSphereRadius(100.0f);
  SphereArea->SetCollisionProfileName(TEXT("Trigger"));
  SphereArea->SetupAttachment(Mesh);

  // Button Widget
  ButtonWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Button"));
  ButtonWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
  ButtonWidget->SetWorldScale3D(FVector(.2f, .2f, .2f));
  ButtonWidget->SetupAttachment(RootComponent);

  // Set the Particle to the mesh
  ComponentInMap = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MaskParticle"));
  ComponentInMap->SetupAttachment(Mesh);

  // Set the Particle to the mesh
  ComponentVanish = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VanishParticle"));
  ComponentVanish->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AHP_A_Mask::BeginPlay() {
  Super::BeginPlay();

  // Get reference to the Camera
  AHP_GM_LocalMultiplayer* gameMode = (AHP_GM_LocalMultiplayer*)GetWorld()->GetAuthGameMode();
  if (gameMode) {
    camera = gameMode->camera;
  }

  // Hide the Widget
  if (ButtonWidget) {
    ButtonWidget->SetHiddenInGame(true);
  }

  // Set the particle
  if (ParticleInMap) {
    ComponentInMap->SetTemplate(ParticleInMap);
  }
  if (VanishParticle) {
    ComponentVanish->SetTemplate(VanishParticle);
  }

  // Save the Initial Pos
  initialPosition = this->GetActorLocation();

  // Deactivate Particles
  ActivateParticle(VanishParticle, ComponentVanish, false);

  setTaked(false, true);
}

void AHP_A_Mask::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  // Update always the widget rotation
  if (!isTaked() && camera) {
    FRotator widgetRot = UKismetMathLibrary::FindLookAtRotation(ButtonWidget->GetComponentLocation(), camera->Camera->GetComponentLocation());
    ButtonWidget->SetWorldRotation(widgetRot);
  }
}

void AHP_A_Mask::PostInitializeComponents() 
{
  Super::PostInitializeComponents();

  SphereArea->OnComponentBeginOverlap.AddDynamic(this, &AHP_A_Mask::OnBeginOverlap);
  SphereArea->OnComponentEndOverlap.AddDynamic(this, &AHP_A_Mask::OnEndOverlap);
}

void AHP_A_Mask::setTaked(bool isTaked, bool disableParticle) {
  bTaked = isTaked;

  switch (isTaked) {
  case true: {
    if (false == disableParticle) {
      // Deactivate Particles
      ActivateParticle(ParticleInMap, ComponentInMap, !isTaked);

      // Deactivate Particles
      ActivateParticle(VanishParticle, ComponentVanish, !isTaked);
    }

    // Deactivate Physics
    if (MaskName != "Fish") {
      Mesh->SetSimulatePhysics(!isTaked);
      Mesh->SetEnableGravity(!isTaked);
    }
    else {
      Mesh->SetSimulatePhysics(true);
      Mesh->SetEnableGravity(true);
    }

    // Deactivate Collision
    if (MaskName != "Fish") {
      Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
      Mesh->SetCollisionProfileName(TEXT("NoCollision"));
    }
    else {
      Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
      Mesh->SetCollisionProfileName(TEXT("FishPreset"));
    }

    // Activate the Overlap Events
    SphereArea->bGenerateOverlapEvents = !isTaked;

  }break;
  case false: {

    if (returnIntialPos) {
      // Return to initial position
      this->SetActorLocation(initialPosition + FVector(0.f, 0.f, 10.f));
      SphereArea->SetRelativeLocation(FVector::ZeroVector);
      SphereArea->SetWorldLocation(initialPosition + FVector(0.f, 0.f, 10.f));
    }
    if (launchOnDetach) {
      // Launch the mask
      ApplyRandomImpulseAtDetach();
    }

    // Activate Physics
    Mesh->SetSimulatePhysics(!isTaked);
    Mesh->SetEnableGravity(!isTaked);

    // Activate Collision
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));

    if (false == disableParticle) {
      // Activate Particles
      ActivateParticle(VanishParticle, ComponentVanish, !isTaked);
      // Activate Particles
      ActivateParticle(ParticleInMap, ComponentInMap, !isTaked);
    }

    // Deactivate the Overlap Events
    SphereArea->bGenerateOverlapEvents = !isTaked;

  }break;
  }
}

bool AHP_A_Mask::isTaked() {
  return bTaked;
}

void AHP_A_Mask::GiveMaskToPlayer(AHP_CH_Player* whichPlayer)
{
  if (ensure(whichPlayer)) {
    // If the mask is different to self mask
    if (whichPlayer->mask != this) {
      // If the player has mask
      if (whichPlayer->mask != nullptr)
      {
        // Call that BP func before the mask will dequiped
        BeforeTheMaskIsDequipped();

        if (whichPlayer->mask->isDefaultMask == false) {
          //Detach the mask from this player
          whichPlayer->mask->DetachAllSceneComponents((USceneComponent*)whichPlayer->GetMesh(), FDetachmentTransformRules::KeepWorldTransform);

          // The old mask will be available
          whichPlayer->mask->setTaked(false);

          // Set the Owner to null
          whichPlayer->mask->owner_ = nullptr;
        }
        else {
          //Destroy the default mask
          whichPlayer->mask->Destroy();
        }

        // Call this func After the mask is dequipped
        AfterTheMaskIsDequipped();
      }

      if (whichPlayer->GetMesh()->DoesSocketExist(SocketToAttach))
      {
        //This mask is taked
        setTaked(true);

        //Attach the mask
        AttachToComponent(whichPlayer->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketToAttach);

        //Rotate the mask
        this->SetActorRotation(whichPlayer->GetActorRotation());

        //Give a reference to this mask
        whichPlayer->mask = this;

        // Set the Owner
        owner_ = whichPlayer;

        //Call the func in bp
        AfterTheMaskIsEquipped();
      }
    }
  }
}

void AHP_A_Mask::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
  
  //Only showed if it's not taked
  if (false == isTaked()) {
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
      // If the Actor is a Player
      if (OtherActor->IsA(AHP_CH_Player::StaticClass())) {

        auto player = Cast<AHP_CH_Player>(OtherActor);
        player->maskCanPickup = this;

        // If exist the widget
        if (ButtonWidget) {
          ButtonWidget->SetHiddenInGame(false);
        }
      }
    }
  }
}

void AHP_A_Mask::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex) {
  //If all it's correct
  if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
    // If the Actor is a Player
    if (OtherActor->IsA(AHP_CH_Player::StaticClass())) {

      auto player = Cast<AHP_CH_Player>(OtherActor);
      player->maskCanPickup = nullptr;

      // Hide always on EndOverlap
      if (ButtonWidget) {
        ButtonWidget->SetHiddenInGame(true);
      }
    }
  }


}

// TRUE = Enable Particle / FALSE = Disable Particle
void AHP_A_Mask::ActivateParticle(UParticleSystem* particle, UParticleSystemComponent* component, bool state) {
  switch (state) {
  case true:
    if (particle) {
      component->ActivateSystem();
      component->SetHiddenInGame(false);
    }
    break;
  case false:
    if (particle) {
      component->DeactivateSystem();
      component->SetHiddenInGame(true);
    }
    break;
  }
}

void AHP_A_Mask::ApplyRandomImpulseAtDetach() {
  int aux_rand_x = FMath::RandRange(1, 2);

  int aux_rand_y = FMath::RandRange(1, 2);
  // Getting random horizontal orientations. CL
  aux_rand_x == 2 ? aux_rand_x = -1 : aux_rand_x = 1;
  aux_rand_y == 2 ? aux_rand_y = -1 : aux_rand_y = 1;

  FVector impulse_vector = FVector(1.0f, 1.0f, 1.0f);

  // Getting random magnitude for more random impulse in X axis. CL
  int32 aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.X = ((1000 * aux_rand_x) * 10.f) +
    ((aux_magnitude_rand_x * 10.f) * aux_rand_x);

  // Getting random magnitude for more random impulse in Y axis. CL
  aux_magnitude_rand_x = FMath::RandRange(4000, 9000);
  impulse_vector.Y = ((1000 * aux_rand_y) * 10.f) +
    ((aux_magnitude_rand_x * 10.f) * aux_rand_y);

  // Getting random magnitude for more random impulse in Z axis. CL
  int32 aux_magnitude_rand_y = FMath::RandRange(7000, 13000);
  impulse_vector.Z = (7000 * 10.f) +
    (aux_magnitude_rand_y * 4.f);

  Mesh->AddImpulse(impulse_vector);
}