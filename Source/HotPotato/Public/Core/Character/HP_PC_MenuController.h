#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"

#include "HP_PC_MenuController.generated.h"

class AHP_A_JukeBoxMenu;
class AHP_PC_MenuController;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwapSelectedCharacter, AHP_PC_MenuController*, controller, int, index);

DECLARE_DELEGATE_OneParam(FOnSwapSelectedButton, int);

UENUM(BlueprintType)
enum class EStateCharacterSelection : uint8
{
  E_None 	              UMETA(DisplayName = "None"),
  E_AtMainMenu          UMETA(DisplayName = "At Main Menu"),
  E_ReadyToJoin 	      UMETA(DisplayName = "Ready To Join"),
  E_SelectingCharacter 	UMETA(DisplayName = "E_Selecting Character"),
  E_ReadyToPlay 	      UMETA(DisplayName = "Ready To Play")
};


/**
 * 
 */
UCLASS()
class HOTPOTATO_API AHP_PC_MenuController : public APlayerController
{
	GENERATED_BODY()
	
 public:
  AHP_PC_MenuController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  
  virtual void BeginPlay() override;

  virtual void Possess(APawn* pawn) override;

  virtual void SetupInputComponent() override;

  UFUNCTION(BlueprintCallable, Category="Menu Controller")
    FORCEINLINE bool HasJoinedTheGame();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Controller")
    EStateCharacterSelection State = EStateCharacterSelection::E_None;

  UPROPERTY()
    AHP_A_JukeBoxMenu* JukeBox = nullptr;

  UPROPERTY(BlueprintAssignable)
    FOnSwapSelectedCharacter OnSwapSelectedCharacterDelegate;


  FOnSwapSelectedButton OnSwapSelectedButtonDelegate;

protected:

  UFUNCTION()
    virtual void Select();

  UFUNCTION()
    virtual void Back();

  UFUNCTION()
    virtual void Navigate(float value);

  UFUNCTION()
    virtual void NavigateThrowSkins(float value);

  UFUNCTION()
    virtual void NavigateThrowButtons(float value);
};

FORCEINLINE bool AHP_PC_MenuController::HasJoinedTheGame()
{
  return State > EStateCharacterSelection::E_ReadyToJoin;
}