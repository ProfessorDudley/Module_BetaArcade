#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "HP_PC_PlayerController.generated.h"

class AHP_CH_Player;
/**
 * 
 */
UCLASS()
class HOTPOTATO_API AHP_PC_PlayerController : public APlayerController
{
  GENERATED_BODY()

 public:

   AHP_PC_PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  virtual void BeginPlay() override;

  virtual void Possess(APawn* pawn) override;

  virtual void SetupInputComponent() override;


 private:

   UFUNCTION()
    void MoveForward(float val);

   UFUNCTION()
    void MoveRight(float val);

   UFUNCTION()
     void Punch();

   UFUNCTION()
     void Dash();

   UFUNCTION()
     void PickupMask();

   UFUNCTION()
     void TugOfWar(float val);

  UPROPERTY()
    AHP_CH_Player* ControlledCharacter;

  /////////////// TUG OF WAR ///////////////

  bool TOWLastWasLeft = false;

  /////////////// TUG OF WAR ///////////////


};
