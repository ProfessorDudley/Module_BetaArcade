// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HP_AI_CharAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOTPOTATO_API UHP_AI_CharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
  public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Punch")
      bool bIsPunching;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Diving")
      bool bIsDiving;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Dashing")
      bool bIsDashing;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|PickedUpMic")
      bool bMicPickupTrigger;

    

  private:
	
	
};
