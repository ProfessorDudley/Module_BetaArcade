// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HP_A_MapSettings.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HP_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOTPOTATO_API UHP_GameInstance : public UGameInstance
{
  GENERATED_BODY()

  UHP_GameInstance(const FObjectInitializer& ObjectInitializer);

public:
  UPROPERTY()
    // Get the lobby MapSettings
    FMapSettings lobbyMapSettigns;

  UPROPERTY()
    // List of all MapSettings
    TArray<FMapSettings> mapSettings;


  UPROPERTY()
    bool JuckboxAlreadyLoaded = false;
};
