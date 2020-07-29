#include "HP_PC_PlayerController.h"

#include "HP_GM_LocalMultiplayer.h"
#include "Core/Character/HP_CH_Player.h"

AHP_PC_PlayerController::AHP_PC_PlayerController(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
    bAutoManageActiveCameraTarget = false;
}

void AHP_PC_PlayerController::BeginPlay()
{
  Super::BeginPlay();

}
void AHP_PC_PlayerController::Possess(APawn* pawn)
{
  Super::Possess(pawn);

  ControlledCharacter = Cast<AHP_CH_Player>(GetPawn());

}
void AHP_PC_PlayerController::SetupInputComponent()
{
  Super::SetupInputComponent();

  check(InputComponent);

  InputComponent->BindAxis("Forward", this, &AHP_PC_PlayerController::MoveForward);
  InputComponent->BindAxis("Strafe", this, &AHP_PC_PlayerController::MoveRight);
  InputComponent->BindAxis("ToW", this, &AHP_PC_PlayerController::TugOfWar);
  InputComponent->BindAction("Punch", IE_Pressed, this, &AHP_PC_PlayerController::Punch);
  InputComponent->BindAction("Dash", IE_Pressed, this, &AHP_PC_PlayerController::Dash);
  InputComponent->BindAction("Mask", IE_Pressed, this, &AHP_PC_PlayerController::PickupMask);

}

void AHP_PC_PlayerController::MoveForward(float val)
{
  
  if (ensure(ControlledCharacter) && val != 0.0f)
    ControlledCharacter->MoveForward(val);
}

void AHP_PC_PlayerController::MoveRight(float val)
{

  if (ensure(ControlledCharacter) && val != 0.0f)
    ControlledCharacter->MoveRight(val);
}

void AHP_PC_PlayerController::Punch()
{
  if (ensure(ControlledCharacter))
    ControlledCharacter->Punch();
}

void AHP_PC_PlayerController::Dash()
{
  if (ensure(ControlledCharacter)) 
    ControlledCharacter->Dash();
}

void AHP_PC_PlayerController::PickupMask()
{
  if (ensure(ControlledCharacter))
    ControlledCharacter->PickupMask();
}

void AHP_PC_PlayerController::TugOfWar(float val) {

  if (ControlledCharacter){
    if (ControlledCharacter->InTugOfWar) {
      // This could probably be an instance variable to avoid LF and casting it multiple times. Cri L.
      AHP_GM_LocalMultiplayer* gm = Cast<AHP_GM_LocalMultiplayer>(GetWorld()->GetAuthGameMode());
      if (true == TOWLastWasLeft && (0.8f < val)) {
        gm->TugOfWarEvent.Broadcast(ControlledCharacter);
        TOWLastWasLeft = false;
      } else if (false == TOWLastWasLeft && (-0.8f > val)) {
        gm->TugOfWarEvent.Broadcast(ControlledCharacter);
        TOWLastWasLeft = true;
      }
    }

  }
}
