#pragma once

#include "HP_A_Mask.h"
#include "HP_A_PickableActor.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "HP_CH_Player.generated.h"

class UHP_AC_ConeComponent;

class UBoxComponent;
class UShapeComponent;
class UWidgetComponent;

struct FCorridorEffectData;
struct FEffectData;


UENUM(Blueprintable)
enum class EEffect : uint8
{
  E_None 	      UMETA(DisplayName = "None"),
  E_Slow 	      UMETA(DisplayName = "Slow"),
  E_KnockBack 	UMETA(DisplayName = "Knock Back"),
  E_KnockUp 	  UMETA(DisplayName = "Knock Up"),
  E_Stun 	      UMETA(DisplayName = "Stun")
};


USTRUCT(BlueprintType)
struct FEffectBase
{
  GENERATED_BODY()

  FEffectBase() = default;
  FEffectBase(const FEffectBase& Other);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
   EEffect Type = EEffect::E_None;
  //The duration of the effect, doen,t applied in knocks effects. Is in seconds.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Punch")
   float Duration = 0.0f;
  /**
  *
  *  The quantity of the effect. 
  * Slow is between [0.0 - 1.0]
  * Knockback is force, it takes the velocity as an influence, take care of it.
  *  
  **/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player", meta = (ClampMin = 0.0))
   float Factor = 0.0f;

 
  UPROPERTY(BlueprintReadOnly, Category = "Player")
    class AHP_CH_Player* WhoAppliedEffect = nullptr;

  bool operator != (const EEffect& other)
  {
    return Type != other;
  }
  bool operator == (const EEffect& other)
  {
    return Type == other;
  }
  bool operator >= (const EEffect& other)
  {
    return Type >= other;
  }
  bool operator <= (const EEffect& other)
  {
    return Type <= other;
  }
  bool operator > (const EEffect& other)
  {
    return Type > other;
  }
  bool operator < (const EEffect& other)
  {
    return Type < other;
  }
};

/**
 *
 * Para los que pregunten por esta gran "cosa" es debido a que unreal
 * no permite la recursividad en estructuras, si hubiera puesto este 
 * array TArray<FEffectData> en la estructura anterior al no haber determinado 
 * su tamaño exacto el compilador no es capaz de seguir creando 
 * un bucle infinito. Sin embarbo al heredar de la otra clase/ estructura 
 * el compilador ya ha determinado su tamaño pudiendo ahora colocar dicha array.
 *
**/
USTRUCT(BlueprintType)
struct FEffectData : public FEffectBase
{
  GENERATED_BODY()

  FEffectData() = default;
  FEffectData(const FEffectBase& other);
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
   bool bActivateAfterEffectsAtTheEnd = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
   TArray<FEffectBase> EffectsAppliedOnEnd = TArray<FEffectBase>();
};

USTRUCT(BlueprintType)
struct FSkillBase
{
  GENERATED_BODY()

    //How many impulse is done when is used.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    float Impulse = 1000.0f;

  //Skill's cooldown
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player", meta = (ClampMin = 0.0))
    float CD = 1.0f;

  //Effect applied.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    FEffectData Effect;

  //How much time is alive the collision.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player", meta=(ClampMin=0.01))
    float LiveSpanTriggerCollision = 1.2f;

  FTimerHandle CoolDownHandle;
  FTimerHandle TriggerLiveHandle;
};

USTRUCT(Blueprintable)
struct FPunchData : public FSkillBase
{
  GENERATED_BODY()

  FPunchData() = default;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player", meta= (ClampMin = 0.0))
   float time_to_hit_ = 0.25f;

  FTimerHandle hit_in_time_handle_;
};

USTRUCT(Blueprintable)
struct FDashData : public FSkillBase
{
  GENERATED_BODY()

  FDashData() = default;

  //Channel collision, DON'T TOUCH IT!!!
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
   TEnumAsByte<ECollisionChannel> CollisionChannel;

};

UCLASS()
class HOTPOTATO_API AHP_CH_Player : public ACharacter
{
	GENERATED_BODY()

public:
	AHP_CH_Player();
  //When you call this function with true it will throw a timer with the lifespan to deactivate it.
  UFUNCTION(BlueprintCallable, Category = "Player")
    void SetActiveTriggerCollision(FSkillBase& skill, bool enable);

  UFUNCTION(BlueprintCallable, Category = "Player")
    FORCEINLINE FEffectData LastEffectApplied();

  UFUNCTION(BlueprintCallable, Category = "Player")
    FORCEINLINE AHP_CH_Player* WhoAppliedLastEffect();

  UFUNCTION(BlueprintCallable, Category = "Player")
    virtual void ApplyEffect(const FEffectData Data);

  UFUNCTION(BlueprintCallable, Category = "Player")
    virtual void RemoveEffect(const FEffectData Data);

  UFUNCTION(BlueprintCallable, Category = "Player")
    virtual void Punch();

  UFUNCTION(BlueprintCallable, Category = "Player")
    virtual void PickupMask();

  UFUNCTION(BlueprintCallable, Category = "Player")
    FORCEINLINE bool HasMaskEquipped();

  UFUNCTION(BlueprintCallable, Category = "Player")
    virtual void Dash();

  /*Return a value [0.0f - 1.0f]*/
  UFUNCTION(BlueprintCallable, Category = "Player")
    float DashCDTimeRemaining();

  /*Return a value [0.0f - 1.0f]*/
  UFUNCTION(BlueprintCallable, Category = "Player")
    float PunchCDTimeRemaining();

  UFUNCTION(BlueprintCallable, Category = "Player")
    bool CanDash();

  UFUNCTION(BlueprintCallable, Category = "Player")
    bool CanPunch();

  UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsPunchValid();

  UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsSlowed();

  UFUNCTION(BlueprintCallable, Category = "Player")
    bool IsStunned();

  UFUNCTION(BlueprintCallable, Category = "Player")
    FORCEINLINE bool IsHoldingMic();

  UFUNCTION(BlueprintCallable, Category = "Player")
    void DropTheMic();

  UFUNCTION(BlueprintCallable, Category = "Player")
    void DropTheMicWithImpulse(FVector dir);

  UFUNCTION()
    virtual void MoveForward(float Value);

  UFUNCTION()
    virtual void MoveRight(float Value);

  UFUNCTION()
    void OnLandAfterEffectApplied(const FHitResult& Hit);

  /////////////// TUG OF WAR ///////////////

   UFUNCTION(BlueprintCallable, Category = "Player")
     FORCEINLINE bool CanEnterTugOfWar();

  /////////////// TUG OF WAR ///////////////



  //~============================================================
  /// Variables
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Punch")
    FPunchData PunchData;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Dash")
    FDashData DashData;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UBoxComponent* PunchShape = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* DashBackCollision = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Pickable")
    AHP_A_PickableActor* PickedUpObjectRef = nullptr; // THIS HAS TO BE SET TO NULL WHEN DROPPING PICKABLES

  
  /////////////// TUG OF WAR ///////////////
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Tug Of War")
    bool InTugOfWar = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Tug Of War")
    float TimeToEnterTugOfWar = 5.0f;

 FTimerHandle TugOfWarTimerHandle;


  /*************************************************/
  /************ Temporal components ****************/
  /*************************************************/
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
    UWidgetComponent* PunchCD;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
    UWidgetComponent* DashCD;

  /*================================================*/
  /*=================== Mask =======================*/
  /*================================================*/
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Mask")
    AHP_A_Mask* mask = nullptr;

  UPROPERTY()
    AHP_A_Mask* maskCanPickup = nullptr;

protected:
	virtual void BeginPlay() override;

  virtual void OnConstruction(const FTransform& transform) override;
  
  virtual void PostInitializeComponents() override;

  UFUNCTION()
   virtual void SetupOverlapEvents();

  UFUNCTION()
   void OnPunchBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;

private:
  UFUNCTION()
    FVector ProjectVectorToFloor(const FVector& Direction);

  UPROPERTY()
   FEffectData LastEffect;

  

  FTimerDelegate StunDelegate;
  FTimerDelegate SlowDelegate;

  FTimerHandle StunnedHandle;
  FTimerHandle SlowHandle;
};


FORCEINLINE bool AHP_CH_Player::HasMaskEquipped()
{
  return mask != nullptr;
}

FORCEINLINE bool AHP_CH_Player::IsHoldingMic()
{
  return PickedUpObjectRef != nullptr;
}

FORCEINLINE bool AHP_CH_Player::CanEnterTugOfWar()
{
  return GetWorldTimerManager().IsTimerActive(TugOfWarTimerHandle);
}

FORCEINLINE AHP_CH_Player* AHP_CH_Player::WhoAppliedLastEffect()
{ 
  return LastEffect.WhoAppliedEffect; 
}

FORCEINLINE FEffectData AHP_CH_Player::LastEffectApplied()
{ 
  return LastEffect; 
}