// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HP_A_Camera.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HP_A_MapSettings.generated.h"


USTRUCT()
struct FMapSettings
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    int32 id;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    bool isValid;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FString name;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FName CurrentMapName;

  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector mapSettingsPos;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector MicPos;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector Point1;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector Point2;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector Point3;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FVector Point4;
  UPROPERTY(EditAnywhere, Category = "FMapSettings")
    FCameraConfiguration cameraConfig;
};

UCLASS()
class HOTPOTATO_API AHP_A_MapSettings : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHP_A_MapSettings();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
  UPROPERTY()
    bool isValid = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // Name of the map
    FString name = FString("MapName");

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // Name of the map
    FName CurrentMapName = "";

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    bool isLobby = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // First Spawn Point
    UStaticMeshComponent* MicrophonePoint;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // First Spawn Point
    UStaticMeshComponent* SpawnPoint1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // Second Spawn Point
    UStaticMeshComponent* SpawnPoint2;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // Third Spawn Point
    UStaticMeshComponent* SpawnPoint3;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
    // Fourth Spawn Point
    UStaticMeshComponent* SpawnPoint4;

  UPROPERTY(EditAnywhere, Category = "MapSettings|CameraMap")
    FCameraConfiguration cameraConfig;
  	
private:
  int32 id;
  FMapSettings getMappSettingsConfig();
};
