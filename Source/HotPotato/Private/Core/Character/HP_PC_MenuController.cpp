// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_PC_MenuController.h"

#include "HP_A_JukeBoxMenu.h"
#include "HP_UW_JukeBoxWidgetBase.h"

#include "Kismet/GameplayStatics.h"


AHP_PC_MenuController::AHP_PC_MenuController(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
  : Super(ObjectInitializer)
{
}

void AHP_PC_MenuController::BeginPlay()
{
  Super::BeginPlay();
}

void AHP_PC_MenuController::Possess(APawn* pawn)
{
  Super::Possess(pawn);

}

void AHP_PC_MenuController::SetupInputComponent()
{
  Super::SetupInputComponent();

  check(InputComponent);

  InputComponent->BindAction("Select", IE_Pressed, this, &AHP_PC_MenuController::Select);
  InputComponent->BindAction("Back", IE_Pressed, this, &AHP_PC_MenuController::Back);
  InputComponent->BindAxis("Strafe", this, &AHP_PC_MenuController::Navigate);
}

void AHP_PC_MenuController::Select()
{
  check(JukeBox);

  if (JukeBox->IsSequenceActive())
  {
    JukeBox->SkipSequence();
    return;
  }

  auto* widget = JukeBox->CurrentActiveWidget();

  if (widget->OnSelect.IsBound())
    widget->OnSelect.Broadcast();

  switch (State)
  {
  case EStateCharacterSelection::E_None:
    break;

  case EStateCharacterSelection::E_AtMainMenu:
    JukeBox->SelectCurrectFocussedButton();
    break;

  case EStateCharacterSelection::E_ReadyToJoin:
  {
    JukeBox->PlayerJoin(this);
    State = EStateCharacterSelection::E_SelectingCharacter;

    break;
  }
  case EStateCharacterSelection::E_SelectingCharacter:
  {
    State = EStateCharacterSelection::E_ReadyToPlay;
    JukeBox->CheckStartGame();
    break;
  }
  case EStateCharacterSelection::E_ReadyToPlay:
    if (UGameplayStatics::GetPlayerControllerID(this) == 0)
    {
      JukeBox->StartGame();
    }
    break;

  default:
    break;
  }
}

void AHP_PC_MenuController::Back()
{
  check(JukeBox);

  auto* widget = JukeBox->CurrentActiveWidget();
  if (widget)
  {
    if (widget->OnBack.IsBound())
      widget->OnBack.Broadcast();
  }
  

  switch (State)
  {
  case EStateCharacterSelection::E_None:
    break;

  case EStateCharacterSelection::E_AtMainMenu:
    break;

  case EStateCharacterSelection::E_ReadyToJoin:
    break;

  case EStateCharacterSelection::E_SelectingCharacter:
  {
    JukeBox->PlayerRemove(this);
    State = EStateCharacterSelection::E_ReadyToJoin;
  }
    break;
  case EStateCharacterSelection::E_ReadyToPlay:
  {
    State = EStateCharacterSelection::E_SelectingCharacter;
    JukeBox->CheckStartGame();
    break;
  }
  default:
    break;
  }
}

void AHP_PC_MenuController::Navigate(float value)
{
  switch (State)
  {
  case EStateCharacterSelection::E_AtMainMenu:
    NavigateThrowButtons(value);
    break;

  case EStateCharacterSelection::E_SelectingCharacter:
    NavigateThrowSkins(value);
    break;

  default:
    break;
  }
 
}

void AHP_PC_MenuController::NavigateThrowSkins(float value)
{
  if (value < -0.5f)
  {
    OnSwapSelectedCharacterDelegate.Broadcast(this, -1);
  }
  else if (value > 0.5f) {
    OnSwapSelectedCharacterDelegate.Broadcast(this, 1);
  }
}

void AHP_PC_MenuController::NavigateThrowButtons(float value)
{
  if (value < -0.5f)
  {
    OnSwapSelectedButtonDelegate.ExecuteIfBound(-1);
  }
  else if (value > 0.5f) {
    OnSwapSelectedButtonDelegate.ExecuteIfBound(1);
  }
}
