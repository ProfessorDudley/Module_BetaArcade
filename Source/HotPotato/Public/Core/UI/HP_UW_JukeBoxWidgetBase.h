// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HP_UW_JukeBoxWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectDeletegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackDeletegate);

class AHP_A_JukeBoxMenu;
class UWidget;
/**
 * 
 */
UCLASS()
class HOTPOTATO_API UHP_UW_JukeBoxWidgetBase : public UUserWidget
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintNativeEvent, Category = "Jukebox")
    UWidget* GetFirstFocusWidget();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JukeBox")
   AHP_A_JukeBoxMenu* JukeBox;
	
  UPROPERTY(BlueprintAssignable, Category = "JukeBox")
   FOnSelectDeletegate OnSelect;

  UPROPERTY(BlueprintAssignable, Category = "JukeBox")
   FOnBackDeletegate OnBack;
};
