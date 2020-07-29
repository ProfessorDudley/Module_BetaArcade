#include "HP_A_JukeBoxMenu.h"

#include "HP_UW_JukeBoxWidgetBase.h"
#include "HP_PC_MenuController.h"
#include "HP_GM_LocalMultiplayer.h"

#if WITH_EDITOR

#include "Misc/MessageDialog.h"
#include "Internationalization/Text.h"

#endif

#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "TimerManager.h"
#include "UserWidget.h"
#include "WidgetBlueprintLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogJukeBox, Log, All);

// Sets default values
AHP_A_JukeBoxMenu::AHP_A_JukeBoxMenu()
{
	PrimaryActorTick.bCanEverTick = false;

  JukeBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void AHP_A_JukeBoxMenu::Init()
{
  SpawnPlayerController(PlayerControllerOnSelection, 4);

  InitializeButtons();

  DisplayMenu(FirstMenuToDisplay, true);
}

void AHP_A_JukeBoxMenu::DisplayCharacterSelection(FName Name)
{
  ChangeControllersState(EStateCharacterSelection::E_ReadyToJoin);

  if (ensure(StarterCamera))
    PlayersControllers[0]->SetViewTargetWithBlend((AActor*)StarterCamera);

  DisplayMenu(Name, true);

}

void AHP_A_JukeBoxMenu::DisplayMenu(FName Name, bool WithSequence /*= true*/, bool PlayBack /*= false*/)
{
  MenuToDisplay = Name;

#if WITH_EDITOR

  bool exist = Sequences.Contains(Name);
  UE_CLOG(!exist, LogJukeBox, Warning, TEXT("Menu's name %s to display doesn,t exist, watch out!!!!"), *Name.ToString ());
  //Move this to the header if needed

  

  if (!exist)
  {
    FMessageDialog::Debugf(FText::FromString("Menu's name to display doesn,t exist, watch out!!!!"));
    return;
  }

#endif

  if (WithSequence)
  {
    FJukeBoxData data = Sequences[Name];

    if (!SequencePlayer)
    {
      SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld (), 
                                                                      data.Sequence, 
                                                                      data.PlaybackSettings,
                                                                      SequenceActor);

      SequencePlayer->OnFinished.AddDynamic(this, &AHP_A_JukeBoxMenu::DisplayMenuSaved);
    }
    else
    {
      SequencePlayer->Initialize(data.Sequence, GetWorld(), data.PlaybackSettings);
    }
    
    !PlayBack ? SequencePlayer->Play() : SequencePlayer->PlayReverse();

  }
  else
  {
    DisplayMenuSaved();
  }
}

void AHP_A_JukeBoxMenu::DisplayBackMenu(FName Name, bool WithSequence /*= true*/, bool PlayBack /*= false*/)
{
  FJukeBoxData& data = Sequences[LastWidgetDisplayed];

  if (data.WidgetOnBack != NAME_None)
  {
    DisplayMenu(data.WidgetOnBack, WithSequence, PlayBack);
  }
}

void AHP_A_JukeBoxMenu::InitializeButtons()
{
  for (auto& button : Buttons)
  {
    FJukeBoxButtonData* data = &button.Value;

    data->MeshComponent = Cast<UStaticMeshComponent>(data->Button->GetComponentByClass(UStaticMeshComponent::StaticClass()));

    if (!data->bIsActive)
    {

#if WITH_EDITOR
      if (!ensure(data->OnDisabled))
      {
        FMessageDialog::Debugf(FText::FromString("Disabled material in button " + data->Button->GetName() + " is null"));
        continue;
      }
#endif

      data->MeshComponent->SetMaterial(0, data->OnDisabled);
    }
    else
    {

#if WITH_EDITOR
      if (!ensure(data->InitMaterial))
      {
        FMessageDialog::Debugf(FText::FromString("Init material in button " + data->Button->GetName() + " is null"));
        continue;
      }
#endif

      data->MeshComponent->SetMaterial(0, data->InitMaterial);
    }
  }

#if WITH_EDITOR
  //Check if the init button index exist.
  if (!Buttons.Contains(CurrentButtonIndex))
  {
    FMessageDialog::Debugf(FText::FromString("Juke box does not contain an init button index as the initial index.,"));
    return;
  }
#endif

  {
    //Set the current focused button.
    FJukeBoxButtonData* data = &Buttons[CurrentButtonIndex];

    data->MeshComponent->SetMaterial(0, data->OnHover);
  }
  
}

void AHP_A_JukeBoxMenu::SelectCurrectFocussedButton()
{
#if WITH_EDITOR
  if (!Buttons.Contains(CurrentButtonIndex))
  {
    FMessageDialog::Debugf(FText::FromString("Selected button is not included in the juke box. " + CurrentButtonIndex));
    return;
  }
#endif

  FJukeBoxButtonData* data = &Buttons[CurrentButtonIndex];

  DisplayMenu(data->MenuDisplayedOnClick, data->bWithSequence, data->bPlayBack);

  ChangeControllersState(data->ControllersStateOnClick);
}

void AHP_A_JukeBoxMenu::RemoveCurrentDisplayedWidget()
{
  UE_LOG(LogJukeBox, Log, TEXT("Remove current widget from viewport."));

  FJukeBoxData& data = Sequences[LastWidgetDisplayed];

  data.WidgetCreated->RemoveFromParent();
}

void AHP_A_JukeBoxMenu::DestroyMenuControllers()
{
  UE_LOG(LogJukeBox, Log, TEXT("Destroy all menu controllers"));

  for (auto controller : PlayersControllers)
  {
    UGameplayStatics::RemovePlayer(controller, true);
  }
}

void AHP_A_JukeBoxMenu::SetActiveCharacterSelectionSpawn(bool enable)
{
  for (auto selectableSpawn : SelectableCharacters)
  {
    AActor* spawn = selectableSpawn.Value;
    spawn->SetActorHiddenInGame(!enable);
    spawn->SetActorTickEnabled(enable);
    spawn->SetActorEnableCollision(enable);
  }
}

void AHP_A_JukeBoxMenu::ChangeControllersState(EStateCharacterSelection newState)
{
  if (newState == EStateCharacterSelection::E_None)
    return;

  for (auto* controller : PlayersControllers)
  {
    controller->State = newState;
  }
}

void AHP_A_JukeBoxMenu::SpawnPlayerController(const TSubclassOf<APlayerController> ControllerToSpawn, int32 Num)
{
  check(Num > 0);
  check(PlayerControllerOnSelection);

  PlayersControllers.Reserve(Num);

  auto currentGameMode = UGameplayStatics::GetGameMode(GetWorld());
  TSubclassOf<APlayerController> lastController = currentGameMode->PlayerControllerClass;
  currentGameMode->PlayerControllerClass = PlayerControllerOnSelection;

  UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), 0), false);

  for (int i = 0; i < Num; i++)
  {
    APlayerController* controller = UGameplayStatics::CreatePlayer(GetWorld(), i, true);

    if (controller->IsA(AHP_PC_MenuController::StaticClass()))
    {
      AHP_PC_MenuController* menu_controller = Cast<AHP_PC_MenuController>(controller);
      menu_controller->JukeBox = this;
      menu_controller->OnSwapSelectedCharacterDelegate.AddDynamic(this, &AHP_A_JukeBoxMenu::SwapCharacterMesh);
      menu_controller->State = EStateCharacterSelection::E_AtMainMenu;

      FJukeBoxPlayerData player_data;
      if (SelectableCharacters.Contains(i))
      {
        AActor* actor = SelectableCharacters[i];

        if (ensure(actor))
        {
          player_data.SkeletalComponent = Cast<USkeletalMeshComponent>(actor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
          player_data.SkeletalComponent->SetActive(false);
          player_data.SkeletalComponent->SetHiddenInGame(true);

          PlayerSelectionData.Add(menu_controller, player_data);
        }
      }

      PlayersControllers.Add(menu_controller);
    }
  }

  PlayersControllers[0]->OnSwapSelectedButtonDelegate.BindUFunction(this, FName("SwapFocusButton"));

  currentGameMode->PlayerControllerClass = lastController;

  if (ensure(StarterCamera))
    PlayersControllers[0]->SetViewTargetWithBlend((AActor*)StarterCamera);
}

void AHP_A_JukeBoxMenu::SwapCharacterMesh(AHP_PC_MenuController* controller, int index)
{
  if (controller->State != EStateCharacterSelection::E_SelectingCharacter)
    return;

  FJukeBoxPlayerData* data = &PlayerSelectionData[controller];

  if (GetWorldTimerManager().IsTimerActive(data->cd))
    return;

  data->MeshIndex += index;

  UE_LOG(LogJukeBox, Log, TEXT("El jugador %d cambia a la skin %d"), UGameplayStatics::GetPlayerControllerID(controller), data->MeshIndex);

  if (data->MeshIndex < 0)
    data->MeshIndex = AvailableSkins.Num() - 1;

  if (data->MeshIndex > AvailableSkins.Num() - 1)
    data->MeshIndex = 0;

  data->SkeletalComponent->SetSkeletalMesh (AvailableSkins[data->MeshIndex].Mesh);

  GetWorldTimerManager().SetTimer(data->cd, 0.5f, false, 0.2f);
}

void AHP_A_JukeBoxMenu::SwapFocusButton(int dir)
{
  if (GetWorldTimerManager().IsTimerActive(ButtonSwapCD))
    return;
  //TODO improve this.
  if (!Buttons[CurrentButtonIndex + dir >= 0 ? CurrentButtonIndex + dir : 0].bIsActive)
    return;

#if WITH_EDITOR
  if (!Buttons.Contains(CurrentButtonIndex))
    return;
#endif

  //Last button return to the init material
  {
    FJukeBoxButtonData* button = &Buttons[CurrentButtonIndex];

    check(button->MeshComponent);

    button->MeshComponent->SetMaterial(0, button->InitMaterial);
  }

  CurrentButtonIndex += dir;

  if (CurrentButtonIndex < 0)
    CurrentButtonIndex = Buttons.Num() - 1;

  if (CurrentButtonIndex > Buttons.Num() - 1)
    CurrentButtonIndex = 0;

#if WITH_EDITOR
  if (!Buttons.Contains(CurrentButtonIndex))
    return;
#endif

  //Next button change material to OnHover
  {
    FJukeBoxButtonData* button = &Buttons[CurrentButtonIndex];

    check(button->MeshComponent);
    
    button->MeshComponent->SetMaterial(0, button->OnHover);
  }

  GetWorldTimerManager().SetTimer(ButtonSwapCD, 0.1f, false, 0.2f);
}

void AHP_A_JukeBoxMenu::PlayerJoin(APlayerController* controller)
{
  if (PlayerSelectionData.Contains(controller))
  {
    FJukeBoxPlayerData* data = &PlayerSelectionData[controller];

    check(data->SkeletalComponent);
    

    data->SkeletalComponent->SetActive(true);
    data->SkeletalComponent->SetHiddenInGame(false);
    data->SkeletalComponent->SetSkeletalMesh(AvailableSkins[0].Mesh);
    PlayersJoined++;

    UE_LOG(LogJukeBox, Log, TEXT("Player %d have joined the game"), UGameplayStatics::GetPlayerControllerID(controller));

  }
}

void AHP_A_JukeBoxMenu::PlayerRemove(APlayerController* controller)
{
  if (PlayerSelectionData.Contains(controller))
  {
    FJukeBoxPlayerData* data = &PlayerSelectionData[controller];

    check(data->SkeletalComponent);

    data->SkeletalComponent->SetActive(false);
    data->SkeletalComponent->SetHiddenInGame(true);
    PlayersJoined--;

    UE_LOG(LogJukeBox, Log, TEXT("Player %d have left the game"), UGameplayStatics::GetPlayerControllerID (controller));
  }
}

void AHP_A_JukeBoxMenu::CheckStartGame()
{
  int8 playersReadyToPlay = 0;

  for (auto controller : PlayersControllers)
  {
    if (controller->State == EStateCharacterSelection::E_ReadyToPlay)
      playersReadyToPlay++;
  }

  bPlayerReadyToPlay = (playersReadyToPlay == PlayersJoined);

  if (DelayTimeToStartGame > 0.0f)
  {
    if (playersReadyToPlay == PlayersJoined)
    {
      //BegiPlay
      UE_LOG(LogJukeBox, Log, TEXT("Begin play in %f"), DelayTimeToStartGame);

      GetWorldTimerManager().SetTimer(StartGameHandle, this, &AHP_A_JukeBoxMenu::StartGame, 0.01f, false, DelayTimeToStartGame);
    }
    else
    {
      if (GetWorldTimerManager().IsTimerActive(StartGameHandle))
      {
        UE_LOG(LogJukeBox, Log, TEXT("Stop start game"));

        GetWorldTimerManager().PauseTimer(StartGameHandle);
      }
    }
  }
  
}

bool AHP_A_JukeBoxMenu::AreAllPlayerReadyToPlay()
{
  return bPlayerReadyToPlay;
}

void AHP_A_JukeBoxMenu::StartGame()
{

  TArray<TSubclassOf<AHP_CH_Player>> playerSelected;
  TArray<APlayerController*> controllers;
  PlayerSelectionData.GetKeys(controllers);

  for (int i = 0; i < PlayersJoined; i++)
  {
    int characterID = PlayerSelectionData[controllers[i]].MeshIndex;
    playerSelected.Add(AvailableSkins[characterID].Player);
  }

  // Get the GameMode
  AHP_GM_LocalMultiplayer* gameMode = Cast<AHP_GM_LocalMultiplayer>(GetWorld()->GetAuthGameMode());

  if (ensure(gameMode)) 
  {
    DestroyMenuControllers();
    RemoveCurrentDisplayedWidget();
    SetActiveCharacterSelectionSpawn(false);

    gameMode->InitPlayers(playerSelected);
  }
  
}

float AHP_A_JukeBoxMenu::RemainingTimeToStartGame()
{
  return GetWorldTimerManager().GetTimerRemaining(StartGameHandle);
}

void AHP_A_JukeBoxMenu::BeginPlay()
{
	Super::BeginPlay();

  //Get GameInstance
  UHP_GameInstance* GameInstance = Cast<UHP_GameInstance>(GetGameInstance());

  SpawnPlayerController(PlayerControllerOnSelection, 4);
  InitializeButtons();

  if (GameInstance->JuckboxAlreadyLoaded == false) {
    DisplayMenu(FirstMenuToDisplay, true);
    GameInstance->JuckboxAlreadyLoaded = true;
  }
  else {
    DisplayCharacterSelection("MainToCharacter");
  }

}

void AHP_A_JukeBoxMenu::DisplayMenuSaved()
{

  //TBI Display Widget
  FJukeBoxData& data = Sequences[MenuToDisplay];

  if (!data.WidgetCreated)
  {
    data.WidgetCreated = Cast<UHP_UW_JukeBoxWidgetBase>(UUserWidget::CreateWidgetOfClass(data.Widget, nullptr, GetWorld(), PlayersControllers[0]));
    data.WidgetCreated->JukeBox = this;
  }
 
  data.WidgetCreated->AddToViewport();

  if (LastWidgetDisplayed != NAME_None)
  {
    FJukeBoxData data = Sequences[LastWidgetDisplayed];

    data.WidgetCreated->RemoveFromParent();
  }

  LastWidgetDisplayed = MenuToDisplay;
}
