// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_A_MapSettings.h"

#include "Kismet/GameplayStatics.h"
#include "HP_GM_LocalMultiplayer.h"
#include "HP_GameInstance.h"

// Sets default values
AHP_A_MapSettings::AHP_A_MapSettings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = root;

    MicrophonePoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Microphone Point"));
    SpawnPoint1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Point 1"));
    SpawnPoint2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Point 2"));
    SpawnPoint3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Point 3"));
    SpawnPoint4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Point 4"));

    // Attach to the Scene component
    MicrophonePoint->SetupAttachment(RootComponent);
    SpawnPoint1->SetupAttachment(RootComponent);
    SpawnPoint2->SetupAttachment(RootComponent);
    SpawnPoint3->SetupAttachment(RootComponent);
    SpawnPoint4->SetupAttachment(RootComponent);
    
    // Set Relative Location to root
    MicrophonePoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
    SpawnPoint1->SetRelativeLocation(FVector(-200.0f, -200.0f, 0.0f));
    SpawnPoint2->SetRelativeLocation(FVector(-200.0f, 200.0f, 0.0f));
    SpawnPoint3->SetRelativeLocation(FVector(200.0f, 200.0f, 0.0f));
    SpawnPoint4->SetRelativeLocation(FVector(200.0f, -200.0f, 0.0f));

    // Hidde the meshes in game
    MicrophonePoint->bHiddenInGame = true;
    SpawnPoint1->bHiddenInGame = true;
    SpawnPoint2->bHiddenInGame = true;
    SpawnPoint3->bHiddenInGame = true;
    SpawnPoint4->bHiddenInGame = true;

    //Disable Collisions
    MicrophonePoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SpawnPoint1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SpawnPoint2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SpawnPoint3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SpawnPoint4->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MicrophonePoint->SetCollisionProfileName(TEXT("OverlapAll"));
    SpawnPoint1->SetCollisionProfileName(TEXT("OverlapAll"));
    SpawnPoint2->SetCollisionProfileName(TEXT("OverlapAll"));
    SpawnPoint3->SetCollisionProfileName(TEXT("OverlapAll"));
    SpawnPoint4->SetCollisionProfileName(TEXT("OverlapAll"));

    MicrophonePoint->bGenerateOverlapEvents = false;
    SpawnPoint1->bGenerateOverlapEvents = false;
    SpawnPoint2->bGenerateOverlapEvents = false;
    SpawnPoint3->bGenerateOverlapEvents = false;
    SpawnPoint4->bGenerateOverlapEvents = false;
}

// Called when the game starts or when spawned
void AHP_A_MapSettings::BeginPlay()
{
    Super::BeginPlay();

    // Get the name of the Level
    CurrentMapName = this->GetLevel()->GetOuter()->GetFName();

    //Get GameInstance
    UHP_GameInstance* GameInstance = Cast<UHP_GameInstance>(GetGameInstance());
    bool isAlreadyLoaded = false;

    // Check if this map is already loaded one time
    if (GameInstance->lobbyMapSettigns.CurrentMapName == CurrentMapName) {
      isAlreadyLoaded = true;
    }
    else {
      for (int i = 0; i < GameInstance->mapSettings.Num(); ++i) {
        if (GameInstance->mapSettings[i].CurrentMapName == CurrentMapName) {
          isAlreadyLoaded = true;
        }
      }
    }

    if (isAlreadyLoaded == false) {
      id = this->GetUniqueID();
      isValid = true;


      // Get the GameMode
      AHP_GM_LocalMultiplayer* gameMode = (AHP_GM_LocalMultiplayer*)GetWorld()->GetAuthGameMode();

      if (gameMode && GameInstance) {
        if (true == isLobby) {
          //Save this info to GameInstance
          GameInstance->lobbyMapSettigns = getMappSettingsConfig();
        }
        else {
          //Save this info to GameInstance
          GameInstance->mapSettings.Add(getMappSettingsConfig());

          // UnLoad the other sublevels
          FLatentActionInfo LatentInfo;
          LatentInfo.UUID = id + 10;
          LatentInfo.Linkage = 0;
          UGameplayStatics::UnloadStreamLevel(this, CurrentMapName, LatentInfo);
        }
      }
    }
}

FMapSettings AHP_A_MapSettings::getMappSettingsConfig()
{
  FMapSettings mapConfig;
  mapConfig.id = id;
  mapConfig.isValid = isValid;
  mapConfig.name = name;
  mapConfig.CurrentMapName = CurrentMapName;
  mapConfig.mapSettingsPos = GetActorLocation();
  mapConfig.MicPos = MicrophonePoint->GetComponentLocation();
  mapConfig.Point1 = SpawnPoint1->GetComponentLocation();
  mapConfig.Point2 = SpawnPoint2->GetComponentLocation();
  mapConfig.Point3 = SpawnPoint3->GetComponentLocation();
  mapConfig.Point4 = SpawnPoint4->GetComponentLocation();
  mapConfig.cameraConfig = cameraConfig;

  return mapConfig;
}
