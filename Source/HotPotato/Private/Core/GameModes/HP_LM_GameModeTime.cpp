// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_LM_GameModeTime.h"


AHP_LM_GameModeTime::AHP_LM_GameModeTime() {

}

void AHP_LM_GameModeTime::BeginPlay()
{
    Super::BeginPlay();

}

void AHP_LM_GameModeTime::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (gameEnded == false) {
        actualTime += DeltaTime;
        if (actualTime >= maxTime) {
            // Set the actualTime equal to maxTime
            actualTime = maxTime;


            // Finish the Round
            FinishRound();

            // Go to the Next Map
            //NextMap();
        }
    }
}


void AHP_LM_GameModeTime::UpdateFadeCode()
{
    Super::UpdateFadeCode();

    actualTime = 0.0f;
}
