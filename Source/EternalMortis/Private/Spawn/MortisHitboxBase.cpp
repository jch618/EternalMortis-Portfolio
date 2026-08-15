#include "Spawn/MortisHitboxBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/MortisCharacterBase.h"
#include "MortisDebugHelper.h"

#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Character/Player/MortisPlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"

AMortisHitboxBase::AMortisHitboxBase()
{
    PrimaryActorTick.bCanEverTick = false;

    HitboxCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitboxCollisionBox"));
    SetRootComponent(HitboxCollisionBox);
    HitboxCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    HitboxCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HitboxNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    HitboxNiagaraComponent->SetupAttachment(GetRootComponent());

    HitboxCascadeComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("CascadeComponent"));
    HitboxCascadeComponent->SetupAttachment(GetRootComponent());
}

void AMortisHitboxBase::OnSpawned(const FGameplayEffectSpecHandle& EffectSpecHandle)
{
    InitializeHitbox(EffectSpecHandle);
}

void AMortisHitboxBase::BeginPlay()
{
    Super::BeginPlay();
    
    HitboxCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnHitboxBeginOverlap);
    HitboxCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnHitboxEndOverlap);

    if (HitboxDamageEffectSpecHandle.IsValid())
    {
        InitializeHitbox(HitboxDamageEffectSpecHandle);
    }
    
    if (HitboxNiagaraComponent && HitboxNiagaraComponent->GetAsset())
    {
        HitboxNiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnNiagaraSystemFinished);
    }
    if (HitboxCascadeComponent && HitboxCascadeComponent->Template)
    {
        HitboxCascadeComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnParticleSystemFinished);
    }
    
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(CollisionTimerHandle, this, &ThisClass::OnCollisionTimerExpired, CollisionActiveDuration);
    }
}

void AMortisHitboxBase::InitializeHitbox(const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (bHasInitialized)
    {
        return;
    }

    HitboxDamageEffectSpecHandle = DamageSpecHandle;
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    if (GetInstigator())
    {
        HitboxCollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);
    }

    bHasInitialized = true;
    
    BP_OnInitializedFX();
}

void AMortisHitboxBase::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* HitPawn = Cast<APawn>(OtherActor);
    if (!Cast<AMortisPlayerCharacter>(HitPawn))
    {
        return;
    }

    if (!HitboxDamageEffectSpecHandle.IsValid())
    {
        MORTIS_LOG("HitboxDamageEffectSpecHandle is invalid on %s", *GetActorNameOrLabel());
        return;
    }
    
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
    if (!ASC)
    {
        return;
    }
    
    const UGameplayEffect* GEDef = HitboxDamageEffectSpecHandle.Data->Def;
    if (!GEDef)
    {
        return;
    }
    if (GEDef->DurationPolicy == EGameplayEffectDurationType::Instant)
    {
        if (HitActors.Contains(OtherActor))
        {
            return;
        }
        HitActors.AddUnique(OtherActor);
        ASC->ApplyGameplayEffectSpecToSelf(*HitboxDamageEffectSpecHandle.Data.Get());
    }
    else
    {
        FActiveGameplayEffectHandle ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*HitboxDamageEffectSpecHandle.Data.Get()); 
        ActiveEffectHandles.Add(OtherActor, ActiveEffectHandle);
    }
    
    BP_OnHitboxOverlappedFX(SweepResult.ImpactPoint);    
}

void AMortisHitboxBase::OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (FActiveGameplayEffectHandle* ActiveEffectHandle = ActiveEffectHandles.Find(OtherActor))
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
        {
            ASC->RemoveActiveGameplayEffect(*ActiveEffectHandle);
            ActiveEffectHandles.Remove(OtherActor);
        }
    }
}

void AMortisHitboxBase::OnNiagaraSystemFinished(UNiagaraComponent* PSystem)
{
    Destroy();
}

void AMortisHitboxBase::OnParticleSystemFinished(UParticleSystemComponent* PSystem)
{
    Destroy();
}

void AMortisHitboxBase::OnCollisionTimerExpired()
{
    HitboxCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HitActors.Empty();
    
    for (const TPair<TObjectPtr<AActor>, FActiveGameplayEffectHandle>& Pair : ActiveEffectHandles)
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pair.Key))
        {
            ASC->RemoveActiveGameplayEffect(Pair.Value);
        }
    }
    ActiveEffectHandles.Empty();
}
