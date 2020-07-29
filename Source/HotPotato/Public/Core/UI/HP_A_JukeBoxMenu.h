
#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"

#include "HP_PC_MenuController.h"

#include "HP_A_JukeBoxMenu.generated.h"

class UHP_UW_JukeBoxWidgetBase;
class AHP_CH_Player;

class ALevelSequenceActor;
class ASceneCapture2D;
class ULevelSequence;
class ULevelSequencePlayer;
class USkeletalMesh;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FJukeBoxButtonData
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MenuDisplayedOnClick = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWithSequence = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPlayBack = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStateCharacterSelection ControllersStateOnClick = EStateCharacterSelection::E_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Button = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActive = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* InitMaterial = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* OnHover = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* OnClick = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* OnDisabled = nullptr;

  UPROPERTY()
    UStaticMeshComponent* MeshComponent = nullptr;
};

USTRUCT(BlueprintType)
struct FJukeBoxSkinsData
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
   USkeletalMesh* Mesh = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
   TSubclassOf<AHP_CH_Player> Player = nullptr;
};

//TODO Hace falta?
USTRUCT(BlueprintType)
struct FJukeBoxPlayerData
{
  GENERATED_BODY()

  UPROPERTY()
   int MeshIndex = 0;

  UPROPERTY()
   USkeletalMeshComponent* SkeletalComponent = nullptr;

  FTimerHandle cd;
};

USTRUCT(BlueprintType)
struct FJukeBoxData
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   ULevelSequence* Sequence;

  //Widget to display when press back button.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
    FName WidgetOnBack = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   TSubclassOf<UHP_UW_JukeBoxWidgetBase> Widget;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   FMovieSceneSequencePlaybackSettings PlaybackSettings;

  UPROPERTY()
    UHP_UW_JukeBoxWidgetBase* WidgetCreated = nullptr;
};


UCLASS()
class HOTPOTATO_API AHP_A_JukeBoxMenu : public AActor
{
	GENERATED_BODY()
	
public:	
	AHP_A_JukeBoxMenu();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void Init();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void DisplayCharacterSelection(FName Name);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void DisplayMenu(FName Name, bool WithSequence = true, bool PlayBack = false);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void DisplayBackMenu(FName Name, bool WithSequence = true, bool PlayBack = false);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void InitializeButtons();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void SelectCurrectFocussedButton();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void RemoveCurrentDisplayedWidget();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void DestroyMenuControllers();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void SetActiveCharacterSelectionSpawn(bool enable);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void ChangeControllersState(EStateCharacterSelection newState);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void SpawnPlayerController(const TSubclassOf<APlayerController> ControllerToSpawn, int32 Num);
  
  UFUNCTION(BlueprintCallable, Category = "JukeBox")
   void SwapCharacterMesh(AHP_PC_MenuController* controller, int index);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void SwapFocusButton(int dir);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void PlayerJoin(APlayerController* controller);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void PlayerRemove(APlayerController* controller);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void CheckStartGame();
  //TODO Put forceinline.
  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    bool AreAllPlayerReadyToPlay();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    void StartGame();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    float RemainingTimeToStartGame();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    FORCEINLINE bool HasPlayerJoinedGame(AHP_PC_MenuController* PlayerController);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    FORCEINLINE bool HasPlayerIdJoinedGame(int id);

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    FORCEINLINE bool IsSequenceActive();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    FORCEINLINE bool SkipSequence();

  UFUNCTION(BlueprintCallable, Category = "JukeBox")
    FORCEINLINE UHP_UW_JukeBoxWidgetBase* CurrentActiveWidget();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   FName FirstMenuToDisplay = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
    ACameraActor* StarterCamera;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   TArray<FJukeBoxSkinsData> AvailableSkins;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   TMap<FName, FJukeBoxData> Sequences;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   TMap<int32, AActor*> SelectableCharacters;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   TSubclassOf<APlayerController> PlayerControllerOnSelection;

  //Does not work now.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
    bool bOverrideCurrentPlayerController = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jukebox")
    float DelayTimeToStartGame = 0.0f;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "JukeBox")
   UStaticMeshComponent* JukeBoxMesh;

  UPROPERTY(VisibleAnywhere)
    TArray<AHP_PC_MenuController*> PlayersControllers;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
    TMap<int, FJukeBoxButtonData> Buttons;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
    uint8 CurrentButtonIndex = 0;

protected:
	virtual void BeginPlay() override;

private:
  UFUNCTION()
    void DisplayMenuSaved();

  UPROPERTY()
    TMap<APlayerController*, FJukeBoxPlayerData> PlayerSelectionData;

  UPROPERTY()
    ULevelSequencePlayer* SequencePlayer = nullptr;

  UPROPERTY()
    ALevelSequenceActor* SequenceActor = nullptr;

  UPROPERTY()
    FName MenuToDisplay = NAME_None;

  UPROPERTY()
    FName LastWidgetDisplayed = NAME_None;

  UPROPERTY()
    int8 PlayersJoined = 0;

  UPROPERTY()
    bool bPlayerReadyToPlay = false;
  

  FTimerHandle ButtonSwapCD;
  FTimerHandle StartGameHandle;
};

FORCEINLINE bool AHP_A_JukeBoxMenu::HasPlayerJoinedGame(AHP_PC_MenuController* PlayerController)
{
  check(PlayerController);

  return PlayerController->HasJoinedTheGame();
}

FORCEINLINE bool AHP_A_JukeBoxMenu::HasPlayerIdJoinedGame(int id)
{
  check(id >= 0 && id < PlayersControllers.Num())

  return PlayersControllers[id]->HasJoinedTheGame();
}

FORCEINLINE UHP_UW_JukeBoxWidgetBase* AHP_A_JukeBoxMenu::CurrentActiveWidget()
{
  if (LastWidgetDisplayed == NAME_None)
    return nullptr;

  return Sequences[LastWidgetDisplayed].WidgetCreated;
}


FORCEINLINE bool AHP_A_JukeBoxMenu::IsSequenceActive()
{
  return SequencePlayer->IsPlaying();
}

FORCEINLINE bool AHP_A_JukeBoxMenu::SkipSequence()
{
  SequencePlayer->GoToEndAndStop();

  return true;
}