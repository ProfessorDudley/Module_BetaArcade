// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Texture.h"
#include <Particles/ParticleSystemComponent.h>
#include "Components/SphereComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HP_A_Mask.generated.h"

class UWidgetComponent;
class AHP_A_Camera;
class AHP_CH_Player;

UCLASS()
class HOTPOTATO_API AHP_A_Mask : public AActor {
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AHP_A_Mask(const class FObjectInitializer& ObjectInitializer);

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void PostInitializeComponents();

  UFUNCTION(BlueprintCallable, Category = "Mask")
    void setTaked(bool isTaked, bool disableParticle = false);
  UFUNCTION(BlueprintCallable, Category = "Mask")
    bool isTaked();

  UFUNCTION(BlueprintCallable, Category = "Mask")
    void GiveMaskToPlayer(AHP_CH_Player* whichPlayer);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    FName MaskName = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    FName SocketToAttach = FName("head");

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mask")
    UWidgetComponent* ButtonWidget;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    UTexture* Image;

  // Mask Mesh
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mask")
    USkeletalMeshComponent *Mesh;

  // Sphere Area
  UPROPERTY(VisibleAnywhere, Category = "Mask")
    USphereComponent* SphereArea;

  // Particle of the Mask
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    UParticleSystem* ParticleInMap;

  UPROPERTY()
    UParticleSystemComponent* ComponentInMap;

  // Particle of the Mask
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    UParticleSystem* VanishParticle;

  UPROPERTY()
    UParticleSystemComponent* ComponentVanish;

  UPROPERTY(EditAnywhere, Category = "Mask|Options")
    bool returnIntialPos = true;

  UPROPERTY(EditAnywhere, Category = "Mask|Options")
    bool launchOnDetach = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mask")
    AHP_CH_Player* owner_ = nullptr;

  UFUNCTION(BlueprintImplementableEvent, Category = "Mask")
    void AfterTheMaskIsEquipped();

  UFUNCTION(BlueprintImplementableEvent, Category = "Mask")
    void BeforeTheMaskIsDequipped();

  UFUNCTION(BlueprintImplementableEvent, Category = "Mask")
    // If you try to access here to Owner probably will be null
    void AfterTheMaskIsDequipped();


  UPROPERTY()
    // To know if it's default
    bool isDefaultMask = false;

protected:
  UPROPERTY()
    AHP_A_Camera* camera;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mask")
    bool bTaked;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mask")
    FVector rotationWhenAttached = FVector::ZeroVector;

  UPROPERTY()
    FVector initialPosition;

private:

  UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  UFUNCTION()
    void ActivateParticle(UParticleSystem* particle, UParticleSystemComponent* component, bool state);

  UFUNCTION()
    void ApplyRandomImpulseAtDetach();
};