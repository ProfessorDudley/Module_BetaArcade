// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "HP_A_PickableActor.generated.h"

class AHP_CH_Player;

UCLASS()
class HOTPOTATO_API AHP_A_PickableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHP_A_PickableActor();
  
  /// ///////// ///
  /// VARIABLES ///
  /// ///////// ///
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Static Mesh Variable")
    UStaticMesh *mesh_variable_;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Socket To Attach Name")
    FName socket_attachment_name_ = TEXT("hand_r");

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Detach")
    float horizontal_magnitude_multiplier_ = 10.0f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Detach")
    float horizontal_random_multiplier_ = 10.0f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Detach")
    float vertical_magnitude_multiplier_ = 10.0f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Detach")
    float vertical_random_multiplier_ = 4.0f;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP_VAR|Detach")
    float TimeUntilCatchableAgain = 1.0f;


  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Static Mesh Component")
    UStaticMeshComponent *my_mesh_;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HP_VAR|Holding Player Reference")
    AHP_CH_Player *HoldingPlayerRef;


  /// END VARIABLES
  /// --------------------------------------------------------------------------

  /// ///////// ///
  /// FUNCTIONS ///
  /// ///////// ///
  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Attach To Player")
    void AttachToPlayerSocket(AActor *actor_to_attach);
  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Attach To Player")
    void DetachFromPlayer();
  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Attach To Player")
    void DetachFromPlayerWithImpulse(FVector dir);
  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Attach To Player")
    bool CanIBeAttachedAgain();

  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Apply Impulse")
    void ApplyRandomImpulseAtDetach();
  UFUNCTION(BlueprintCallable, Category = "HPFUNC|Pickable Actor|Apply Impulse")
    void ApplyImpulseAtDetach(FVector dir);

  /// END FUNCTIONS
  /// --------------------------------------------------------------------------

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
  bool picked_up_;
  FTimerHandle RecatchHandler;
	
};
