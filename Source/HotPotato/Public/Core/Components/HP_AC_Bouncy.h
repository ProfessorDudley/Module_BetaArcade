// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HP_AC_Bouncy.generated.h"


UCLASS(Experimental, ClassGroup=(HotPotato), meta=(BlueprintSpawnableComponent), meta = (ShortTooltip = "A Component that adds bouncy propierties to the actor."))
class HOTPOTATO_API UHP_AC_Bouncy : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHP_AC_Bouncy();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                             FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
