// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_A_LobbyDoor.h"

#include "HP_CH_Player.h"
#include "HP_A_Camera.h"
#include "HP_GM_LocalMultiplayer.h"

#if WITH_EDITOR
#include "Engine.h"
#include "Editor.h"
#endif

// Sets default values
AHP_A_LobbyDoor::AHP_A_LobbyDoor(const class FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer) {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  // Create the Box Area
  BoxArea = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxArea"));
  BoxArea->SetBoxExtent(FVector( 100.f, 100.f, 100.f));
  BoxArea->SetCollisionProfileName(TEXT("Trigger"));
  BoxArea->SetupAttachment(RootComponent);

  // Set the Particle to the mesh
  ComponentNotReady = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleNotReady"));
  ComponentNotReady->SetupAttachment(RootComponent);

  // Set the Particle to the mesh
  ComponentReady = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleReady"));
  ComponentReady->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHP_A_LobbyDoor::BeginPlay()
{
	Super::BeginPlay();

  // Get reference to the Camera
  gameMode = (AHP_GM_LocalMultiplayer*)GetWorld()->GetAuthGameMode();

  // Set the particle
  if (ParticleNotReady) {
    ComponentNotReady->SetTemplate(ParticleNotReady);
  }
  if (ParticleReady) {
    ComponentReady->SetTemplate(ParticleReady);
  }

  // Activate/Deactivate Particles
  ActivateParticle(ParticleNotReady, ComponentNotReady, true); // Active
  ActivateParticle(ParticleReady, ComponentReady, false); // Deactived
}

void AHP_A_LobbyDoor::PostInitializeComponents()
{
  Super::PostInitializeComponents();

  BoxArea->OnComponentBeginOverlap.AddDynamic(this, &AHP_A_LobbyDoor::OnBeginOverlap);
  BoxArea->OnComponentEndOverlap.AddDynamic(this, &AHP_A_LobbyDoor::OnEndOverlap);
}

void AHP_A_LobbyDoor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  //If all it's correct
  if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
    // If the Actor is a Player
    if (OtherActor->IsA(AHP_CH_Player::StaticClass())) {
     
      // Check if this player is added yet
      AHP_CH_Player* overlappedActor = (AHP_CH_Player*)OtherActor;
      bool exist = false;
      for (auto player : playersInArea) {
        if (player == overlappedActor){
          exist = true;
        }
      }

      if (false == exist) {
        playersInArea.Add(overlappedActor);
      }

      // If all the players are in the area
      if (gameMode->playerList.Num() == playersInArea.Num()) {
        bool theyHaveTheMask = true;
        for (auto& player : playersInArea) {
          if (player->mask == nullptr || player->mask->isDefaultMask == true) {
            theyHaveTheMask = false;
          }
        }

        // If all the players in the area have the mask Start the Game
        if (true == theyHaveTheMask && gameMode->startedGame == false && activated == false) {
          activated = true;
          // Deactivate the NotReady and Activate the Ready
          ActivateParticle(ParticleNotReady, ComponentNotReady, false);
          ActivateParticle(ParticleReady, ComponentReady, true);
          // Start the Game
          gameMode->StartGame();
        }
      }

    }
  }
}

void AHP_A_LobbyDoor::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
  //If all it's correct
  if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
    // If the Actor is a Player
    if (OtherActor->IsA(AHP_CH_Player::StaticClass())) {

      // Remove the player reference if he go out the area
      AHP_CH_Player* overlappedActor = (AHP_CH_Player*)OtherActor;
      bool exist = false;
      for (auto player : playersInArea) {
        if (player == overlappedActor) {
          exist = true;
        }
      }

      if (true == exist) {
        activated = false;

        // Remove this player
        playersInArea.Remove(overlappedActor);

        // Particles
        ActivateParticle(ParticleNotReady, ComponentNotReady, true); // Active
        ActivateParticle(ParticleReady, ComponentReady, false); // Deactive
      }

    }
  }
}

// TRUE = Enable Particle / FALSE = Disable Particle
void AHP_A_LobbyDoor::ActivateParticle(UParticleSystem* particle, UParticleSystemComponent* component, bool state)
{
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