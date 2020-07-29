// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_GM_LocalMultiplayer.h"
#include "HP_A_Mask.h"
#include "HP_A_Camera.h"
#include "HP_CH_Player.h"
#include "HP_A_MapSettings.h"
#include "HP_A_PickableActor.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
DEFINE_LOG_CATEGORY_STATIC(LogBaseGameMode, Log, All);

AHP_GM_LocalMultiplayer::AHP_GM_LocalMultiplayer() {

}

void AHP_GM_LocalMultiplayer::BeginPlay()
{
  Super::BeginPlay();

  //Get GameInstance
  GameInstance = Cast<UHP_GameInstance>(GetGameInstance());

  // Create the Widgets we have
  CreateWidgets();

  // Game is Ended
  gameEnded = true;
}

void AHP_GM_LocalMultiplayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (microphone) {
      if (microphone->HoldingPlayerRef != nullptr) {
        AddPointsTo(microphone->HoldingPlayerRef, DeltaTime);
      }
    }

    ///////////////// DEBUG STUFF. Cri L. /////////////////

    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString(TEXT("ToW Actual Score Difference: ")) += FString::FromInt(ToWActualScoreDifference), true);
    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString(TEXT("ToW Actual Score Difference Target: ")) += FString::FromInt(ToWActualScoreTarget), true);
    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, FString(TEXT("ToW Actual Score Difference Delayed: ")) += FString::FromInt(ToWActualScoreDelayed), true);

    if (TOWPlayer2 == nullptr) {
      //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString(TEXT("Player 2 de Tug of War: NULL")), true);
    }
    else {
      //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString(TEXT("Player 2 de Tug of War: ")) += TOWPlayer2->GetName(), true);
    }
    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString(TEXT("Score de ToW de Player 2: ")) += FString::FromInt(TOWPlayer2Score), true);

    if (TOWPlayer1 == nullptr) {
      //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString(TEXT("Player 1 de Tug of War: NULL")), true);
    } else {
      //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString(TEXT("Player 1 de Tug of War: ")) += TOWPlayer1->GetName(), true);
    }
    //GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString(TEXT("Score de ToW de Player 1: ")) += FString::FromInt(TOWPlayer1Score), true);

    //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, "Score de ToW de Player 2: " + TOWPlayer2Score);
    ///////////////// DEBUG STUFF. Cri L. /////////////////

    // Tug Of War //
    if (bInTugOfWar) {
      if (ToWActualScoreDelayed < ToWActualScoreTarget)
        ToWActualScoreDelayed++;
      if (ToWActualScoreDelayed > ToWActualScoreTarget)
        ToWActualScoreDelayed--;
    }

    // End Tug Of War //



}

void AHP_GM_LocalMultiplayer::Init()
{
    // Just in case the camera doesn't exist
    SpawnCamera();

    // Create the Player controllers and spawn the characters
    SpawnPlayers();

    //end_close_level_event_.AddDynamic(this, &ATacticalRPGGameModeBase::EndCloseLevel);
    TugOfWarEvent.AddDynamic(this, &AHP_GM_LocalMultiplayer::TugOfWarHandler);
    TugOfWarStartedEvent.AddDynamic(this, &AHP_GM_LocalMultiplayer::PrepareTugOfWar);
}

void AHP_GM_LocalMultiplayer::InitPlayers(TArray<TSubclassOf<AHP_CH_Player>> players)
{
  maxPlayerCount = players.Num();

  // Just in case the camera doesn't exist
  SpawnCamera();

  // Create the Player controllers and spawn the characters
  TArray<APlayerController*> controllerArray;
  // Create the PlayerControllers
  for (int index = 0; index < players.Num(); index++)
  {
    APlayerController* controller = UGameplayStatics::CreatePlayer(GetWorld(), index, true);
    controllerArray.Add(controller);
  }

  // Spawn default mask
  TArray<AHP_A_Mask*> defaultMaskList;
  if (defaultMask) {
    for (int index = 0; index < players.Num(); index++) {
      AHP_A_Mask* newMask = GetWorld()->SpawnActor<AHP_A_Mask>(defaultMask, FVector::ZeroVector, FRotator::ZeroRotator);
      newMask->isDefaultMask = true;
      defaultMaskList.Add(newMask);
    }
  }

  // Spawn Players
  for (int index = 0; index < players.Num(); index++) 
  {
    // If we have a player blueprint to spawn 
    if (players[index])
    {
      // Get PlayerStart Position
      FVector psLocation = GetLobbyPosition(index);

      // Spawning
      AHP_CH_Player* player = GetWorld()->SpawnActor<AHP_CH_Player>(players[index], psLocation, FRotator(0.0f, 180.0f, 0.0f));

      // Possessing controller with the player
      APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), index);
      playerController->Possess(player);

      // Add player to the array
      playerList.Add(player);

      // Add to wins map and score
      playerWins.Add(player, 0);
      playerScore.Add(player, 0);

      if (defaultMask && defaultMaskList[index]) {
        // Set the default mask to the player
        defaultMaskList[index]->GiveMaskToPlayer(player);
      }
    }
  }

  UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(camera);

  //end_close_level_event_.AddDynamic(this, &ATacticalRPGGameModeBase::EndCloseLevel);
  TugOfWarEvent.AddDynamic(this, &AHP_GM_LocalMultiplayer::TugOfWarHandler);
  TugOfWarStartedEvent.AddDynamic(this, &AHP_GM_LocalMultiplayer::PrepareTugOfWar);
}

void AHP_GM_LocalMultiplayer::SpawnPlayers()
{
  TArray<APlayerController*> controllerArray;
  // Create the PlayerControllers
  for (int index = 1; index < maxPlayerCount; index++) {
    APlayerController* controller = UGameplayStatics::CreatePlayer(GetWorld(), index, true);
    controllerArray.Add(controller);
  }

  // If we have a player blueprint to spawn 
  if (playerToSpawn) {
    // Spawn Players
    for (int index = 0; index < maxPlayerCount; index++) {

      // Get PlayerStart Position
      FVector psLocation = GetLobbyPosition(index);

      // Spawning
      FActorSpawnParameters SpawnParams;
      SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
      AHP_CH_Player* player = GetWorld()->SpawnActor<AHP_CH_Player>(playerToSpawn, psLocation, FRotator(0.0f, 180.0f, 0.0f), SpawnParams);

      // Possessing controller with the player
      APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), index);
      playerController->Possess(player);

      // Add player to the array
      playerList.Add(player);

      // Add to wins map and score
      playerWins.Add(player, 0);
      playerScore.Add(player, 0);
    }
  }

  UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(camera);
}

void AHP_GM_LocalMultiplayer::SpawnCamera()
{
    if (cameraToSpawn) {
        // If not exist a camera, we create
        if (!camera) {
            camera = GetWorld()->SpawnActor<AHP_A_Camera>(cameraToSpawn, FVector(0.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
            camera->SetCameraConfig(GameInstance->lobbyMapSettigns.cameraConfig);
            camera->SetActorLocation(GameInstance->lobbyMapSettigns.mapSettingsPos);
        }
    }
}

void AHP_GM_LocalMultiplayer::SpawnMicrophone()
{
    if (microphoneToSpawn) {
        // If not exist a Microphone, we create
        if (microphone == nullptr) {
            microphone = GetWorld()->SpawnActor<AHP_A_PickableActor>(microphoneToSpawn, FVector(0.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
        }
    }
}

void AHP_GM_LocalMultiplayer::GenerateOrderMap() {
  mapsOrder.Empty();

  int randomIndex = 0;
  TArray<int> mapIndexRandom; 

  for (int i = 0; i < GameInstance->mapSettings.Num(); i++) {
      do {
          randomIndex = FMath::RandRange(0, (GameInstance->mapSettings.Num() - 1));
          // If the index was choosed generate other
      } while (mapIndexRandom.Contains(randomIndex));
      mapIndexRandom.Add(randomIndex);

      mapsOrder.Add(i, GameInstance->mapSettings[randomIndex]);
  }
}

void AHP_GM_LocalMultiplayer::StartGame() {
  // Reset the Score
  ResetPlayerWins();

  // Set the mapIndex to 0
  mapIndex = 0;

  // Generate the Order of the maps
  GenerateOrderMap();

  // Spawn the camera
  SpawnCamera();

  // Spawn a Microphone
  SpawnMicrophone();

  // Set StartGame to true
  startedGame = true;

  //Go to the next map
  NextMap();
}

void AHP_GM_LocalMultiplayer::ResetPlayerScore()
{
    for (auto& player : playerScore) {
        player.Value = 0.0f;
    }
}

void AHP_GM_LocalMultiplayer::ResetPlayerWins()
{
    for (auto& player : playerWins) {
        player.Value = 0;
    }
}

void AHP_GM_LocalMultiplayer::AddPointsTo(AHP_CH_Player* player, float points)
{
    if (player != nullptr) {
        if (true == playerScore.Contains(player)) {
            float pointsToAdd = points * pointsMultiplier;
            playerScore.Add(player, playerScore[player] + pointsToAdd);
        }
    }
}

void AHP_GM_LocalMultiplayer::FinishRound()
{
  // Bool set to true
  gameEnded = true;

  if (mapIndex < mapsOrder.Num()) {
    int index = 0;
    for (auto& player : playerScore) {
      //If has a score better than 0.0f points
      if (player.Value > 0.0f) {
        switch (index) {
        case 0:
          playerWins.Add(player.Key, playerWins[player.Key] + pointsToFirstPlayer);
          break;
        case 1:
          playerWins.Add(player.Key, playerWins[player.Key] + pointsToSecondPlayer);
          break;
        case 2:
          playerWins.Add(player.Key, playerWins[player.Key] + pointsToThirdPlayer);
          break;
        case 3:
          playerWins.Add(player.Key, playerWins[player.Key] + pointsToFourthPlayer);
          break;
        }
      }

      index++;
    }

    // Set the Score Widget
    SetWidget(ScoreHUDRef);
  }
  else {
    // Set the Wins Widget
    SetWidget(WinsHUDRef);
  }
}

void AHP_GM_LocalMultiplayer::SortPlayers()
{
    playerWins.ValueSort([](const int& A, const int& B) { return A > B; });
    playerScore.ValueSort([](const int& A, const int& B) { return A > B; });
}

void AHP_GM_LocalMultiplayer::PrepareTugOfWar(AHP_CH_Player * character1, AHP_CH_Player * character2) {
  // TODO Disable input for other players. Cri L.
  // TODO Disable movement for ToW players. Cri L. -> DONE on the player controller class.
  if (ensureAlways(character1) && ensureAlways(character2)) {
    TOWPlayer1 = character1;
    TOWPlayer1->InTugOfWar = true;
    TOWPlayer1Score = 0;

    TOWPlayer2 = character2;
    TOWPlayer2Score = 0;
    TOWPlayer2->InTugOfWar = true;

    ToWActualScoreTarget = 0;
    ToWActualScoreDifference = 0;
    ToWActualScoreDelayed = 0;

    bInTugOfWar = true;

    if (ensureAlways(TOWPlayer1->PickedUpObjectRef)) {
      TOWPlayer1->DropTheMic();
      //microphone->my_mesh_->SetVisibility(false);
    }
    SnapPlayers();

  }
}

void AHP_GM_LocalMultiplayer::SnapPlayers() {
  //FVector auxp1pos = microphone->GetActorLocation();
  //auxp1pos.Y -= 100.0f;
  //TOWPlayer1->SetActorLocation(auxp1pos);

  FVector micpos = TOWPlayer1->GetActorLocation();
  micpos.Y += 75.0f;
  microphone->SetActorLocation(micpos);

  FVector auxp2pos = TOWPlayer1->GetActorLocation();
  auxp2pos.Y += 150.0f;
  TOWPlayer2->SetActorLocation(auxp2pos);


  FRotator auxp1rot = TOWPlayer1->GetActorRotation();
  FRotator auxp2rot = TOWPlayer2->GetActorRotation();
  auxp1rot.Yaw = 90.0f;
  auxp2rot.Yaw = -90.0f;
  TOWPlayer1->SetActorRotation(auxp1rot);
  TOWPlayer2->SetActorRotation(auxp2rot);
  microphone->my_mesh_->SetEnableGravity(false);
  microphone->my_mesh_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHP_GM_LocalMultiplayer::TugOfWarHandler(AHP_CH_Player* character) {
  if (character->IsValidLowLevelFast()) {
    // Adding score to respective player
    if (TOWPlayer1 == character) {
      TOWPlayer1Score++;
    } else if (TOWPlayer2 == character) {
      TOWPlayer2Score++;
    }
    // Checking if the score is higher than max difference.
    if ((TOWPlayer1Score - TugOfWarMaxScoreDifference) > TOWPlayer2Score) {
      // Player 1 won
      EndTugOfWar(true);
    } else if ((TOWPlayer2Score - TugOfWarMaxScoreDifference) > TOWPlayer1Score) {
      // Player 2 won
      EndTugOfWar(false);
    }

    // Updating internal score variables in order to animate the Tug Of War
    ToWActualScoreDifference = TOWPlayer2Score - TOWPlayer1Score;
    ToWActualScoreTarget = ToWActualScoreDifference * 10;
  } else {
    UE_LOG(LogBaseGameMode, Log, TEXT("The Player passed through parameter in the event is invalid"));
  }
}

void AHP_GM_LocalMultiplayer::EndTugOfWar(bool playerOneWon) {
  
  microphone->DetachFromPlayer();
  //microphone->my_mesh_->SetVisibility(true);
  microphone->my_mesh_->SetEnableGravity(true);
  microphone->my_mesh_->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  bInTugOfWar = false;
  
  if (playerOneWon) {
    // Attaching the mic to the winner.
    microphone->AttachToPlayerSocket(TOWPlayer1);
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Black, FString(TEXT("Player 1 Won the Tug of War")), true);
    // Throwing the losing player away
    FVector aux = FVector(TOWPlayer2->GetActorLocation() - TOWPlayer1->GetActorLocation());
    aux.Normalize();
    TOWPlayer2->LaunchCharacter(aux * 2000.0f, false, false);
  } else {
    // Attaching the mic to the winner.
    microphone->AttachToPlayerSocket(TOWPlayer2);
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Black, FString(TEXT("Player 2 Won the Tug of War")), true);
    // Throwing the losing player away
    FVector aux = FVector(TOWPlayer1->GetActorLocation() - TOWPlayer2->GetActorLocation());
    aux.Normalize();
    TOWPlayer1->LaunchCharacter(aux * 2000.0f, false, false);
  }
  // Reseting Tug Of War Variables
  TOWPlayer1Score = 0;
  TOWPlayer1->InTugOfWar = false;
  TOWPlayer2Score = 0;
  TOWPlayer2->InTugOfWar = false;


  TOWPlayer1 = nullptr;
  TOWPlayer2 = nullptr;
}

void AHP_GM_LocalMultiplayer::EnablePlayerInput(bool enable) {

  // Enable or Disable Player Input
  for (int i = 0; i < playerList.Num(); ++i) {
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), i);

    if (true == enable) {
      controller->EnableInput(controller);
    }
    else {
      controller->DisableInput(controller);
    }
  }
}

void AHP_GM_LocalMultiplayer::NextMap()
{
  // FadeIn
  InitFade();

  //Teleport the players to next map
  UpdateFade();
    
  // Fade Out
  FinishFade();
}

void AHP_GM_LocalMultiplayer::InitFade()
{
  // Get CameraManager
  if (camera) {
    camera->cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
  }

  //Start the Fade
  camera->FadeIn();

  // Disable the Players Input
  EnablePlayerInput(false);
}

void AHP_GM_LocalMultiplayer::UpdateFadeCode()
{
  // Load the next map
  if (mapIndex < mapsOrder.Num()) {
    LoadMap(mapsOrder[mapIndex]);
  }
  else {
    //Get GameInstance
    UHP_GameInstance* GameInstance = Cast<UHP_GameInstance>(GetGameInstance());
    UGameplayStatics::OpenLevel((UObject*)GameInstance, FName(TEXT("Game_P")));
    return;
  }

  // Hide the Current Widget
  HideCurrentWidget();

  // If Microphone exist will be detached
  if (microphone) {
    microphone->DetachFromPlayer();
  }

  //If next map exist
  if (mapIndex < mapsOrder.Num()) {
      // Teleport the camera
      FVector mapPos = mapsOrder[mapIndex].mapSettingsPos;

      // Set Camera Config Values
      camera->SetCameraConfig(mapsOrder[mapIndex].cameraConfig);
      camera->SetActorLocation(mapPos);

      // Move Microphone to the map
      FVector micPos = mapsOrder[mapIndex].MicPos;
      if (microphone) {
        microphone->SetActorLocation(micPos);
      }
        
      // Random Spawn Position
      TMap<int, FVector> randomSpawnPoint;
      for (int i = 0; i < playerList.Num(); ++i) {

          int randNum = 0;
          do {
              randNum = FMath::RandRange(0, playerList.Num() - 1);
          } while (randomSpawnPoint.Contains(randNum));

          switch (i) {
          case 0:
              randomSpawnPoint.Add(randNum, mapsOrder[mapIndex].Point1);
              break;
          case 1:
              randomSpawnPoint.Add(randNum, mapsOrder[mapIndex].Point2);
              break;
          case 2:
              randomSpawnPoint.Add(randNum, mapsOrder[mapIndex].Point3);
              break;
          case 3:
              randomSpawnPoint.Add(randNum, mapsOrder[mapIndex].Point4);
              break;
          }

      }

      // Set player Location in the next map
      switch (playerList.Num()) {
      case 1:
          playerList[0]->SetActorLocationAndRotation(randomSpawnPoint[0], FRotator(0.0f, 180.0f, 0.0f));
        break;
      case 2:
          playerList[0]->SetActorLocationAndRotation(randomSpawnPoint[0], FRotator(0.0f, 180.0f, 0.0f));
          playerList[1]->SetActorLocationAndRotation(randomSpawnPoint[1], FRotator(0.0f, 180.0f, 0.0f));
          break;
      case 3:
          playerList[0]->SetActorLocationAndRotation(randomSpawnPoint[0], FRotator(0.0f, 180.0f, 0.0f));
          playerList[1]->SetActorLocationAndRotation(randomSpawnPoint[1], FRotator(0.0f, 180.0f, 0.0f));
          playerList[2]->SetActorLocationAndRotation(randomSpawnPoint[2], FRotator(0.0f, 180.0f, 0.0f));
          break;
      case 4:
          playerList[0]->SetActorLocationAndRotation(randomSpawnPoint[0], FRotator(0.0f, 180.0f, 0.0f));
          playerList[1]->SetActorLocationAndRotation(randomSpawnPoint[1], FRotator(0.0f, 180.0f, 0.0f));
          playerList[2]->SetActorLocationAndRotation(randomSpawnPoint[2], FRotator(0.0f, 180.0f, 0.0f));
          playerList[3]->SetActorLocationAndRotation(randomSpawnPoint[3], FRotator(0.0f, 180.0f, 0.0f));
          break;
      }
  }
  else {
      // Go to Lobby      
      FVector playerStartPos = GameInstance->lobbyMapSettigns.mapSettingsPos;
      camera->SetCameraConfig(GameInstance->lobbyMapSettigns.cameraConfig);
      camera->SetActorLocation(playerStartPos);

      for (int i = 0; i < playerList.Num(); ++i) {
          FVector psLocation = GetLobbyPosition(i);
          if (i < playerList.Num()) {
              playerList[i]->SetActorLocationAndRotation(psLocation, FRotator(0.0f, 180.0f, 0.0f));
          }
      }

      // Set StartGame to true
      startedGame = false;
  }

  // Reset the player score for the next map
  ResetPlayerScore();
}

void AHP_GM_LocalMultiplayer::UpdateFade()
{
    float delayToFadeOut = camera->FadeTime / 3.0f;

    FTimerHandle tHandle;
    GetWorldTimerManager().SetTimer(tHandle, this, &AHP_GM_LocalMultiplayer::UpdateFadeCode, delayToFadeOut, false);

}

void AHP_GM_LocalMultiplayer::FinishFadeCode()
{
  // UnLoad the next map
  int map = mapIndex - 1;
  if (map >= 0 && map < mapsOrder.Num()) {
    UnLoadMap(mapsOrder[map]);
  }
  else {
    UnLoadMap(GameInstance->lobbyMapSettigns);
  }

  if (!camera) {
    // Spawn the Camera
    SpawnCamera();
    // Get CameraManager
    camera->cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
  }
  camera->FadeOut();

  //The next map
  mapIndex++;
}

void AHP_GM_LocalMultiplayer::FadeEnded() {
  //Enable the Player Input
  EnablePlayerInput(true);

  if ((mapIndex - 1) < mapsOrder.Num()) {
    // Return the bool to false
    gameEnded = false;

    // Set the Game Widget
    SetWidget(GameHUDRef);
  }
  else {
    //Destroy this characters after Game Ends
    DestroyCurrentPlayers();
  }
}

void AHP_GM_LocalMultiplayer::FinishFade()
{
  float delayToFadeOut = camera->FadeTime / 3.0f;
  delayToFadeOut *= 2.0f;

  FTimerHandle tHandle;
  GetWorldTimerManager().SetTimer(tHandle, this, &AHP_GM_LocalMultiplayer::FinishFadeCode, delayToFadeOut, false);

  FTimerHandle fadeEndedTHandle;
  GetWorldTimerManager().SetTimer(fadeEndedTHandle, this, &AHP_GM_LocalMultiplayer::FadeEnded, camera->FadeTime, false);
}

FVector AHP_GM_LocalMultiplayer::GetLobbyPosition(int position) {
    FVector psLocation = FVector(0.0f, 0.0f, 0.0f);
    if (GameInstance->lobbyMapSettigns.isValid == true) {
        switch (position) {
        case 0:
            psLocation = GameInstance->lobbyMapSettigns.Point1;
            break;
        case 1:
            psLocation = GameInstance->lobbyMapSettigns.Point2;
            break;
        case 2:
            psLocation = GameInstance->lobbyMapSettigns.Point3;
            break;
        case 3:
            psLocation = GameInstance->lobbyMapSettigns.Point4;
            break;
        }
    }
    return psLocation;
}


void AHP_GM_LocalMultiplayer::CreateWidgets() {
    if (GameHUD) {
      GameHUDRef = CreateWidget<UUserWidget>(GetWorld(), GameHUD);
    }
    if (ScoreHUD) {
      ScoreHUDRef = CreateWidget<UUserWidget>(GetWorld(), ScoreHUD);
    }
    if (WinsHUD) {
      WinsHUDRef = CreateWidget<UUserWidget>(GetWorld(), WinsHUD);
    }
    HideCurrentWidget();
}

void AHP_GM_LocalMultiplayer::SetWidget(UUserWidget* widgetToSet) {
    if (CurrentWidget != nullptr) {
      // Hide the current Widget
      HideCurrentWidget();
    }

    if (widgetToSet != nullptr) { 
      currentHUDVisible = true;
      CurrentWidget = widgetToSet;
      CurrentWidget->AddToViewport();
    }
}

void AHP_GM_LocalMultiplayer::HideCurrentWidget() {
  if (CurrentWidget != nullptr) {
    currentHUDVisible = false;
    // Hide the current Widget
    CurrentWidget->RemoveFromParent();
  }
}

void AHP_GM_LocalMultiplayer::LoadMap(FMapSettings mapSettings) {
  FLatentActionInfo LatentInfo;
  //LatentInfo.CallbackTarget = this;
  //LatentInfo.ExecutionFunction = FName("UpdateFade"); // Execute this function when the map is loaded
  LatentInfo.UUID = mapSettings.id;
  LatentInfo.Linkage = 0;

  UGameplayStatics::LoadStreamLevel(this, mapSettings.CurrentMapName, true, true, LatentInfo);
}

void AHP_GM_LocalMultiplayer::UnLoadMap(FMapSettings mapSettings) {
  FLatentActionInfo LatentInfo;
  //LatentInfo.CallbackTarget = this;
  //LatentInfo.ExecutionFunction = FName("FinishFade"); // Execute this function when the map is unloaded
  LatentInfo.UUID = mapSettings.id + 10;
  LatentInfo.Linkage = 0;

  UGameplayStatics::UnloadStreamLevel(this, mapSettings.CurrentMapName, LatentInfo);
}


void AHP_GM_LocalMultiplayer::DestroyCurrentPlayers() {

  // Detached from player just in case
  microphone->DetachFromPlayer();

  // Clear the map
  playerWins.Empty();

  // Clear the Map
  playerScore.Empty();

  for (int i = 0; i < playerList.Num(); ++i) {
    //Detach the mask from this player
    if (playerList[i]->mask) {
      playerList[i]->mask->DetachAllSceneComponents((USceneComponent*)playerList[i]->GetMesh(), FDetachmentTransformRules::KeepWorldTransform);
      playerList[i]->mask->setTaked(false);
      playerList[i]->mask->owner_ = nullptr;
    }

    // Remove this Controller
    playerList[i]->GetController()->Destroy();

    // Destroy It
    playerList[i]->Destroy();
  }

  playerList.Empty();
}

int AHP_GM_LocalMultiplayer::MapsNumber() {
  return mapsOrder.Num();
}