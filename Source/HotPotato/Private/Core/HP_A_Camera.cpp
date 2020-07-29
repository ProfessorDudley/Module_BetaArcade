// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_A_Camera.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "HP_GM_LocalMultiplayer.h"
#include "TimerManager.h"


// Sets default values
AHP_A_Camera::AHP_A_Camera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

    RootComponent = SpringArm;
    Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AHP_A_Camera::BeginPlay()
{
	  Super::BeginPlay();
	
    // Set the Default Values
    SetCameraConfig(cameraConfig);

    // Get GameMode
    gameMode = (AHP_GM_LocalMultiplayer*)GetWorld()->GetAuthGameMode();
    gameMode->camera = this;

    // Get Camera Manager
    cameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;        
}

// Called every frame
void AHP_A_Camera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (gameMode->playerList.Num() > 0) {

        // Just the first time
        if (init == false) {
            init = true;
            // Set the Camera at correct Position
            InitialPosition();
        }

        // Check
        bool nullReference = false;
        for (int i = 0; i < gameMode->playerList.Num(); ++i) {
          if (!gameMode->playerList[i]) {
            nullReference = true;
          }
        }

        // Max Distance
        float maxDistance = 0.0f;
        if (!nullReference) {
          // Get Max Distance
          switch (gameMode->maxPlayerCount)
          {
          case 1:
            maxDistance = CalculateZoomP1();
            break;
          case 2:
            maxDistance = CalculateZoomP2();
            break;
          case 3:
            maxDistance = CalculateZoomP3();
            break;
          case 4:
            maxDistance = CalculateZoomP4();
            break;
          }
        }

        // Clam between min and max Zoom
        float result = FMath::Clamp(maxDistance, cameraConfig.minZoom, cameraConfig.maxZoom);

        // Get Actor Array Average Location
        FVector averageLocation = FVector::ZeroVector;
        for (auto player : gameMode->playerList) {
          if (player) {
            averageLocation += player->GetActorLocation();
          }
        }
        FVector targetPos = averageLocation / gameMode->playerList.Num();

        // Lerp the movement
        FVector finalPos = FMath::Lerp(this->GetActorLocation(), targetPos, DeltaTime);

        // Set the middle position
        this->SetActorLocation(finalPos);

        // Set spring arm length
        SpringArm->TargetArmLength = result;

    }
}

void AHP_A_Camera::InitialPosition() {
    if (gameMode->playerList.Num() > 0) {
        float maxDistance = 0.0f;

        // Check
        bool nullReference = false;
        for (int i = 0; i < gameMode->playerList.Num(); ++i) {
          if (!gameMode->playerList[i]) {
            nullReference = true;
          }
        }

        if(!nullReference){
          // Get Max Distance
          switch (gameMode->maxPlayerCount) {
          case 1:
            maxDistance = CalculateZoomP1();
            break;
          case 2:
            maxDistance = CalculateZoomP2();
            break;
          case 3:
            maxDistance = CalculateZoomP3();
            break;
          case 4:
            maxDistance = CalculateZoomP4();
            break;
          }
        }
        

        // Clam between min and max Zoom
        float result = FMath::Clamp(maxDistance, cameraConfig.minZoom, cameraConfig.maxZoom);

        // Get Actor Array Average Location
        FVector averageLocation = FVector::ZeroVector;
        for (auto player : gameMode->playerList) {
          if (player) {
            averageLocation += player->GetActorLocation();
          }
        }
        FVector targetPos = averageLocation / gameMode->playerList.Num();

        // Set the middle position
        this->SetActorLocation(targetPos);

        // Set spring arm length
        SpringArm->TargetArmLength = result;

    }
}

void AHP_A_Camera::SetCameraConfig(FCameraConfiguration newConfig)
{
    cameraConfig = newConfig;

    // Set the Default Values
    SpringArm->bDoCollisionTest = cameraConfig.springArmCollision;
    SpringArm->TargetArmLength = cameraConfig.Length;
    SpringArm->SetRelativeRotation(cameraConfig.CameraRotation);

    SpringArm->TargetOffset = cameraConfig.cameraOffsetPosition;
}

float AHP_A_Camera::CalculateZoomP1()
{
  TArray<float> allDistances;

  //Calculate all Distances between players
  allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[0]->GetActorLocation()));

  // Return the Max Distance
  return FMath::Max<float>(allDistances);
}

float AHP_A_Camera::CalculateZoomP2()
{
    TArray<float> allDistances;

    //Calculate all Distances between players
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[1]->GetActorLocation()));

    // Return the Max Distance
    return FMath::Max<float>(allDistances);
}

float AHP_A_Camera::CalculateZoomP3()
{
    TArray<float> allDistances;

    //Calculate all Distances between players
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[1]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[2]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[1]->GetActorLocation(), gameMode->playerList[2]->GetActorLocation()));

    // Return the Max Distance
    return FMath::Max<float>(allDistances);
}

float AHP_A_Camera::CalculateZoomP4()
{
    TArray<float> allDistances;

    //Calculate all Distances between players
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[1]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[2]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[0]->GetActorLocation(), gameMode->playerList[3]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[1]->GetActorLocation(), gameMode->playerList[2]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[1]->GetActorLocation(), gameMode->playerList[3]->GetActorLocation()));
    allDistances.Add(FVector::Dist(gameMode->playerList[2]->GetActorLocation(), gameMode->playerList[3]->GetActorLocation()));

    // Return the Max Distance
    return FMath::Max<float>(allDistances);
}


void AHP_A_Camera::FadeIn() {
    if (cameraManager) {
        float time = FadeTime / 3.0f;
        cameraManager->StartCameraFade(0.0f, 1.0f, time, colorToFade, true, true);
    }
}

void AHP_A_Camera::FadeOut() {
    if (cameraManager) {
        float time = FadeTime / 3.0f;
        cameraManager->StartCameraFade(1.0f, 0.0f, time, colorToFade, true, true);
    }
}

void AHP_A_Camera::Shake() {
    if (cameraManager) {
        if (cShake) {
            cameraManager->PlayCameraShake(cShake);
        }
    }
}

void AHP_A_Camera::CameraShake(TSubclassOf<UCameraShake> shake)
{
    if (cameraManager) {
        if (shake) {
            cameraManager->PlayCameraShake(shake);
        }
    }
}
