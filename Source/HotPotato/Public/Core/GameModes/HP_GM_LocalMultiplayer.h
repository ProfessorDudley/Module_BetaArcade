// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HP_A_MapSettings.h"
#include "HP_GameInstance.h"
#include "Blueprint/UserWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HP_GM_LocalMultiplayer.generated.h"


class AHP_A_Camera;
class AHP_CH_Player;
class AHP_A_PickableActor;
class AHP_A_Mask;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTugOfWar, AHP_CH_Player*, character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTugOfWarStarted, AHP_CH_Player*, character1, AHP_CH_Player*, character2);


UCLASS()
class HOTPOTATO_API AHP_GM_LocalMultiplayer : public AGameMode
{
  GENERATED_BODY()

public:
  AHP_GM_LocalMultiplayer();

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  // Called every frame
  virtual void Tick(float DeltaTime) override;

  /////////////// BASIC FUNCTIONS ///////////////
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Init the Game
    void Init();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Init the Game
    void InitPlayers(TArray<TSubclassOf<AHP_CH_Player>> players);

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Spawn the Players
    void SpawnPlayers();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Spawn the Players
    void SpawnCamera();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Spawn the Microphone
    void SpawnMicrophone();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Generate a random order map
    void GenerateOrderMap();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Spawn the Players
    void NextMap();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Function Spawn the Players
    void StartGame();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    //Enable or Disable the PlayerInput
    void EnablePlayerInput(bool enable = true);
  /////////////// BASIC FUNCTIONS ///////////////

  /////////////// FADE FUNCTIONS ///////////////
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Start the Fade
    void InitFade();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Call the UpdateFadeCode with a delay (time/3)
    void UpdateFade();
  UFUNCTION(Category = "LocalMultiplayer")
    // Teleport players, camera, etc
    virtual void UpdateFadeCode();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Call the FinishFadeCode with a delay ((time/3) * 2)
    void FinishFade();
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    // Finishing the fade
    void FinishFadeCode();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    void FadeEnded();

  /////////////// FADE FUNCTIONS ///////////////


  /////////////// OTHER FUNCTIONS ///////////////
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
      // Reset the Score in all players
      void ResetPlayerScore();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
      // Reset the Wins in all players
      void ResetPlayerWins();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
      // Add points to X player
      void AddPointsTo(AHP_CH_Player* player, float points);

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
      // Finish the round and update player score
      void FinishRound();
    
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
      void SortPlayers();
  /////////////// OTHER FUNCTIONS ///////////////


  /////////////// TUG OF WAR ///////////////

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Tug Of War")
    // Prepares the game for a Tug of War
    void PrepareTugOfWar(AHP_CH_Player* character1, AHP_CH_Player* character2);

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Tug Of War")
    // Snaps the players facing eachother for launch ToW
    void SnapPlayers();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Tug Of War")
    // Handles the events from tug of wars.
    void TugOfWarHandler(AHP_CH_Player* character);

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Tug Of War")
    // Manages and resolves the ending part of the tug of war.
    void EndTugOfWar(bool playerOneWon);

  /////////////// TUG OF WAR ///////////////


  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer")
      // Player Blueprint to Spawn
      TSubclassOf<AHP_CH_Player> playerToSpawn;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer")
      // Camera Blueprint to Spawn
      TSubclassOf<AHP_A_Camera> cameraToSpawn;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer")
      // Camera Blueprint to Spawn
      TSubclassOf<AHP_A_PickableActor> microphoneToSpawn;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer")
    // Camera Blueprint to Spawn
    TSubclassOf<AHP_A_Mask> defaultMask;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer", meta = (ClampMin = 2.0))
      // The Players count
      int maxPlayerCount = 4;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|References")
      // List of all Players
      TArray<AHP_CH_Player*> playerList;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|References")
      // Reference to the camera
      AHP_A_Camera* camera;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|References")
      // Reference to the Microphone
      AHP_A_PickableActor* microphone;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Map")
      // Index of the Map
      int mapIndex = 0;

  UPROPERTY(VisibleAnywhere, Category = "LocalMultiplayer|Map")
      // The Order of the maps will played
      TMap<int, FMapSettings> mapsOrder;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game")
      // Bool to know when the game is ended
      bool gameEnded = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game")
      // Players Wins Info
      TMap<AHP_CH_Player*, int> playerWins;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Points")
      // Points for the first player
      float pointsMultiplier = 1.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Points")
      // Points for the first player
      int pointsToFirstPlayer = 3;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Points")
      // Points for the second player
      int pointsToSecondPlayer = 2;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Points")
      // Points for the third player
      int pointsToThirdPlayer = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Points")
      // Points for the fourth player
      int pointsToFourthPlayer = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|ActualMap")
      // Players Score Info
      TMap<AHP_CH_Player*, float> playerScore;

  /////////////// TUG OF WAR ///////////////
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Tug Of War")
    int TugOfWarMaxScoreDifference = 5.0f;
  UPROPERTY(BlueprintAssignable)
    FTugOfWar TugOfWarEvent;
  UPROPERTY(BlueprintAssignable)
    FTugOfWarStarted TugOfWarStartedEvent;

  // Animation
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tug Of War Animations")
    // Relative score between players, always calculated as: P2score - P1score
    int ToWActualScoreDifference = 0;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tug Of War Animations")
    // Relative score between players, multiplied by 10 in order to smooth animations.
    int ToWActualScoreTarget = 0;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tug Of War Animations")
    // Delayed score variable that goes up and down slowly over time to reflect the ToW status in animations.
    int ToWActualScoreDelayed = 0;


  /////////////// TUG OF WAR ///////////////

  ///////////////// WIDGETS /////////////////
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Widget")
      void CreateWidgets();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Widget")
      void SetWidget(UUserWidget* widgetToSet);

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Widget")
      void HideCurrentWidget();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets|References")
    UUserWidget* CurrentWidget;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets")
    bool currentHUDVisible = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets")
    TSubclassOf<UUserWidget> GameHUD;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets|References")
    UUserWidget* GameHUDRef;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets")
    TSubclassOf<UUserWidget> ScoreHUD;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets|References")
    UUserWidget* ScoreHUDRef;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets")
    TSubclassOf<UUserWidget> WinsHUD;
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Widgets|References")
    UUserWidget* WinsHUDRef;

  ///////////////// WIDGETS /////////////////

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game")
    bool startedGame = false;

  // Load and Unload Maps
  void LoadMap(FMapSettings mapSettings);
  void UnLoadMap(FMapSettings mapSettings);

  // Destroy Current players
  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer|Players")
    void DestroyCurrentPlayers();

  UFUNCTION(BlueprintCallable, Category = "LocalMultiplayer")
    int MapsNumber();

protected:
  UHP_GameInstance* GameInstance;

private:
  FVector GetLobbyPosition(int position);

  /////////////// TUG OF WAR ///////////////

  AHP_CH_Player* TOWPlayer1 = nullptr; // 1 should always be the mic holder, and placed left on snap animation.
  AHP_CH_Player* TOWPlayer2 = nullptr;
  int TOWPlayer1Score = 0;
  int TOWPlayer2Score = 0;
  bool bInTugOfWar = false;

  /////////////// TUG OF WAR ///////////////
};
