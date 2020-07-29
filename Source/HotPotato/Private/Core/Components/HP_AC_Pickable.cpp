// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_AC_Pickable.h"


// Sets default values for this component's properties
UHP_AC_Pickable::UHP_AC_Pickable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHP_AC_Pickable::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHP_AC_Pickable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

