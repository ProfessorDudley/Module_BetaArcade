// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"

#include "HP_CH_Player.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HP_A_Camera.generated.h"

class AHP_GM_LocalMultiplayer;

// Camera Config
USTRUCT()
struct FCameraConfiguration {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        bool springArmCollision = false;
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        float Length = 1200.0f;
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        float minZoom = 800.0f;
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        float maxZoom = 1500.0f;
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        FRotator CameraRotation = FRotator(-45.0f, 0.0f, 0.0f);
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        FVector cameraOffsetPosition = FVector(-200.0f, 0.0f, 0.0f);
};

UCLASS()
class HOTPOTATO_API AHP_A_Camera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHP_A_Camera();

    UFUNCTION(BlueprintCallable, Category = "CameraSettings|FadeIn", meta = (DisplayName = FadeIn))
        void FadeIn();
    UFUNCTION(BlueprintCallable, Category = "CameraSettings|FadeOut", meta = (DisplayName = FadeOut))
        void FadeOut();

    UFUNCTION(BlueprintCallable, Category = "CameraSettings")
        void Shake();

    UFUNCTION(BlueprintCallable, Category = "CameraSettings")
        void CameraShake(TSubclassOf<UCameraShake> shake);

    UFUNCTION(BlueprintCallable, Category = "CameraSettings")
        // Set the camera in the correct position between the players
        void InitialPosition();

    UFUNCTION(Category = "CameraSettings")
        // Set the camera in the correct position between the players
        void SetCameraConfig(FCameraConfiguration newConfig);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
        USpringArmComponent* SpringArm;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
        UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
        // The Camera Shake values
        TSubclassOf<UCameraShake> cShake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
        //The time in total. Example: 3 in total, 1.0 fade in, 1.0 in black and 1.0 fade out 
        float FadeTime = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings")
        // The Color to Fade
        FLinearColor colorToFade = FLinearColor::Black;

    // SpringArm Data
    UPROPERTY(EditAnywhere, Category = "CameraSettings|Config")
        FCameraConfiguration cameraConfig;

    UPROPERTY()
    APlayerCameraManager* cameraManager;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    float CalculateZoomP1();
    float CalculateZoomP2();
    float CalculateZoomP3();
    float CalculateZoomP4();
    	
private:
    AHP_GM_LocalMultiplayer* gameMode;
    bool fading = false;
    bool init = false;
};
