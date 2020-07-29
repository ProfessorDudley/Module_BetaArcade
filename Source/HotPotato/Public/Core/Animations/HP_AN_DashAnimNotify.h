// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HP_AN_DashAnimNotify.generated.h"

/**
 * DEPRECATED I TRIED BUT I DONT KNOW EXACTLY HOW TO MAKE THIS WORK THE WAY I WANT
 */
UCLASS()
class HOTPOTATO_API UHP_AN_DashAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

  //virtual bool Received_Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequence* AnimSeq) const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP|AN|Dash impulse")
    float ImpulseValue;

  UFUNCTION(BlueprintCallable, Category = "Dash impulse")
    void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


	
	
	
	
};
