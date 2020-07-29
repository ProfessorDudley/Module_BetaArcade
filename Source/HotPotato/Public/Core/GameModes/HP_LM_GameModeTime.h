// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GameModes/HP_GM_LocalMultiplayer.h"
#include "HP_LM_GameModeTime.generated.h"

/**
 * 
 */
UCLASS()
class HOTPOTATO_API AHP_LM_GameModeTime : public AHP_GM_LocalMultiplayer
{
    GENERATED_BODY()
    AHP_LM_GameModeTime();

public:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Adding more code to this func
    void UpdateFadeCode() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Time")
        // The Time of each map
        float maxTime = 60.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LocalMultiplayer|Game|Time")
        // The actual time
        float actualTime = 0.0f;
};
