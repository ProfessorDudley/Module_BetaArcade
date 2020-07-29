// Fill out your copyright notice in the Description page of Project Settings.

#include "HP_AN_DashAnimNotify.h"

void UHP_AN_DashAnimNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {
  GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString("HOLA LLAMO DESDE UN CUSTOM ANIM NOTIFY!"));

}
