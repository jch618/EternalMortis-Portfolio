#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Interfaces/MortisSpawnableInterface.h"
#include "Types/MortisEnumTypes.h"
#include "MortisHitboxBase.generated.h"

struct FGameplayEventData;
class UBoxComponent;
class UNiagaraComponent;
class UParticleSystemComponent;

UCLASS()
class ETERNALMORTIS_API AMortisHitboxBase : public AActor, public IMortisSpawnableInterface
{
    GENERATED_BODY()

public:
    AMortisHitboxBase();

    //~ Begin IMortisSpawnableInterace
    virtual void OnSpawned(const FGameplayEffectSpecHandle& EffectSpecHandle) override;
    //~ End IMortisSpawnableInterace
    
    UFUNCTION(BlueprintCallable, Category = "Mortis|Hitbox")
    void InitializeHitbox(const FGameplayEffectSpecHandle& DamageSpecHandle);


protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent)
    void BP_OnInitializedFX();
    
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UBoxComponent> HitboxCollisionBox;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UNiagaraComponent> HitboxNiagaraComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    TObjectPtr<UParticleSystemComponent> HitboxCascadeComponent;
    
    UPROPERTY(BlueprintReadOnly, Category = "Mortis|Hitbox", meta = (ExposeOnSpawn = "true"))
    FGameplayEffectSpecHandle HitboxDamageEffectSpecHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mortis|Hitbox")
    float CollisionActiveDuration = 1.f;

protected:
    UFUNCTION()
    virtual void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    UFUNCTION()
    virtual void OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION(BlueprintImplementableEvent, Category = "Mortis|Hitbox")
    void BP_OnHitboxOverlappedFX(const FVector& HitLocation);

private:    
    UPROPERTY()
    TArray<TObjectPtr<AActor>> HitActors;
    
    UPROPERTY()
    TMap<TObjectPtr<AActor>, FActiveGameplayEffectHandle> ActiveEffectHandles; 

    bool bHasInitialized = false;
    
    FTimerHandle CollisionTimerHandle;
    
    UFUNCTION()
    void OnNiagaraSystemFinished(UNiagaraComponent* PSystem);
    
    UFUNCTION()
    void OnParticleSystemFinished(UParticleSystemComponent* PSystem);
    
    void OnCollisionTimerExpired();
};