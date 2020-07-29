// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Particles/ParticleSystemComponent.h>
#include "Components/BoxComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HP_A_LobbyDoor.generated.h"

class AHP_CH_Player;
class AHP_GM_LocalMultiplayer;

UCLASS()
class HOTPOTATO_API AHP_A_LobbyDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHP_A_LobbyDoor(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
  virtual void PostInitializeComponents();

  // Sphere Area
  UPROPERTY(VisibleAnywhere, Category = "LobbyDoor")
    UBoxComponent* BoxArea;

  // Particles
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyDoor|Particles")
    UParticleSystem* ParticleNotReady;
  UPROPERTY()
    UParticleSystemComponent* ComponentNotReady;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LobbyDoor|Particles")
    UParticleSystem* ParticleReady;
  UPROPERTY()
    UParticleSystemComponent* ComponentReady;

  // List of the players in the area
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LobbyDoor")
    TArray<AHP_CH_Player*> playersInArea;

  // Reference to the GameMode
  UPROPERTY()
    AHP_GM_LocalMultiplayer* gameMode;

protected:
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LobbyDoor")
    bool activated = false;

private:
  UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  UFUNCTION()
    void ActivateParticle(UParticleSystem* particle, UParticleSystemComponent* component, bool state);
	
	
};
