#include "Spawn/MortisSpawnConfig.h"

#include "MortisDebugHelper.h"
#include "MortisFunctionLibrary.h"
#include "Character/MortisCharacterBase.h"
#include "Character/Enemy/MortisEnemyCharacter.h"
#include "Projectile/MortisProjectileBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Combat/MortisCombatComponent.h"
#include "Spawn/MortisHitboxBase.h"
#include "Spawn/MortisSpawnIndicator.h"
#include "Spawn/MortisSpawnTypes.h"

struct FMortisSpawnEnemyRow;
struct FMortisRoomCustomSpawnRow;

void UMortisSpawnConfig::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
}

void UMortisSpawnConfig::SpawnWithIndicatorOrImmediate(UWorld* World, TSubclassOf<AActor> ClassToSpawn, const FVector& Location, const FRotator& Rotation, const
    FGameplayEffectSpecHandle& DamageSpecHandle, APawn* Instigator)
{
    if (!World || !ClassToSpawn || !Instigator)
    {
        MORTIS_LOG("");
        return;
    }
    
    if (IndicatorClass)
    {
        FActorSpawnParameters Params;
        Params.Instigator = Instigator;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
        AMortisSpawnIndicator* Indicator = World->SpawnActor<AMortisSpawnIndicator>(IndicatorClass, Location, Rotation, Params);        
        if (Indicator)
        {
            Indicator->InitAndStart(ClassToSpawn, DamageSpecHandle, Instigator);
        }
    }
    else
    {
        FActorSpawnParameters Params;
        Params.Instigator = Instigator;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        AActor* Spawned = World->SpawnActor<AActor>(ClassToSpawn, Location, Rotation, Params);
        if (IMortisSpawnableInterface* SpawnableInterface = Cast<IMortisSpawnableInterface>(Spawned))
        {
            SpawnableInterface->OnSpawned(DamageSpecHandle);
        }
    }
}

FTransform UMortisSpawnConfig::GetSpawnTransform(const AMortisCharacterBase* Instigator, const AActor* Target) const
{
    FVector BaseLocation = FVector::ZeroVector;
    FRotator BaseRotation = FRotator::ZeroRotator;

    switch (SpawnLocationType)
    {
    case EMortisSpawnLocationType::InstigatorSocket:
        if (Instigator && Instigator->GetMesh())
        {
            BaseLocation = Instigator->GetMesh()->GetSocketLocation(SpawnSocketName);
            BaseRotation = Instigator->GetMesh()->GetSocketRotation(SpawnSocketName);
        }
        break;
    case EMortisSpawnLocationType::TargetLocation:
        if (Target)
        {
            BaseLocation = Target->GetActorLocation();
            BaseRotation = Target->GetActorRotation();
        }
        break;
    case EMortisSpawnLocationType::InstigatorLocation:
        if (Instigator)
        {
            BaseLocation = Instigator->GetActorLocation();
            BaseRotation = Instigator->GetActorRotation();
        }
        break;
    case EMortisSpawnLocationType::InstigatorWeaponSocket:
        if (AMortisWeaponBase* Weapon = UMortisFunctionLibrary::GetWeaponFromSlot(Instigator, WeaponSlotTag))
        {
            if (UMeshComponent* WeaponMesh = Weapon->GetWeaponMesh())
            {
                BaseLocation = WeaponMesh->GetSocketLocation(SpawnSocketName);
                // MORTIS_LOG("Location: (%f, %f)", BaseLocation.X, BaseLocation.Y);
                BaseRotation = WeaponMesh->GetSocketRotation(SpawnSocketName);
            }
        }
        break;
    }

    FTransform BaseTransform(BaseRotation, BaseLocation);
    
    FVector FinalLocation = BaseTransform.TransformPosition(RelativeOffset);
    
    return FTransform(BaseRotation, FinalLocation);
}

void UMortisSpawnConfig_SingleHitbox::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !HitboxClass)
    {
        return;
    }
    
    const FTransform SpawnTransform = GetSpawnTransform(Instigator, Target);
    const FVector SpawnLocation = SpawnTransform.GetLocation(); 
    const FVector Direction = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    const FRotator SpawnRotation = Direction.Rotation();

    SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), HitboxClass, SpawnLocation, SpawnRotation, DamageSpecHandle, Instigator);
}

void UMortisSpawnConfig_SingleProjectile::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !ProjectileClass)
    {
        return;
    }
    
    const FTransform SpawnTransform = GetSpawnTransform(Instigator, Target);
    const FVector SpawnLocation = SpawnTransform.GetLocation(); 
    const FVector Direction = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    const FRotator SpawnRotation = Direction.Rotation();

    SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), ProjectileClass, SpawnLocation, SpawnRotation, DamageSpecHandle, Instigator);
}

void UMortisSpawnConfig_MultiProjectile::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !ProjectileClass || ProjectileCount <= 0)
    {
        return;
    }

const FTransform SpawnTransform = GetSpawnTransform(Instigator, Target);
    const FVector SpawnLocation = SpawnTransform.GetLocation(); 
    const FVector BaseDirection = (Target->GetActorLocation() - SpawnLocation).GetSafeNormal();
    
    const float HalfSpread = SpreadAngleDegrees * 0.5f;
    const float AngleStep = (ProjectileCount > 1) ? SpreadAngleDegrees / (ProjectileCount - 1) : 0.f;
    
    for (int32 i = 0; i < ProjectileCount; i++)
    {
        const float Angle = (ProjectileCount > 1) ? -HalfSpread + AngleStep * i : 0.f;
        const FVector Direction = BaseDirection.RotateAngleAxis(Angle, FVector::UpVector);

        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), ProjectileClass, SpawnLocation, Direction.Rotation(), DamageSpecHandle, Instigator);
    }
}

void UMortisSpawnConfig_GroundSpawn::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || !Target || !EffectActorClass)
    {
        return;
    }
    
    FActorSpawnParameters Params;
    Params.Instigator = Instigator;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    const FTransform BaseTransform = GetSpawnTransform(Instigator, Target);
    for (const FVector& Offset : RelativeGroundEffectLocations)
    {
        FVector FinalLocation = BaseTransform.TransformPosition(Offset);

        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), EffectActorClass, FinalLocation, BaseTransform.GetRotation().Rotator(), DamageSpecHandle, Instigator);
    }
}

void UMortisSpawnConfig_SummonEnemy::Execute(AMortisCharacterBase* Instigator, const AActor* Target, const FGameplayEffectSpecHandle& DamageSpecHandle)
{
    if (!Instigator || CustomEnemySpawnRowHandle.IsNull())
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.Instigator = Instigator;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    FMortisRoomCustomSpawnRow* CustomSpawnRow = CustomEnemySpawnRowHandle.GetRow<FMortisRoomCustomSpawnRow>(TEXT("SpawnCustomSpawnRow"));
    if (!CustomSpawnRow)
    {
        return;
    }
	
    TArray<FDataTableRowHandle> EnemyRowHandles = CustomSpawnRow->EnemyRows;
    TArray<TSubclassOf<AMortisEnemyCharacter>> EnemiesToSpawn;
    for (const FDataTableRowHandle& EnemyRowHandle : EnemyRowHandles)
    {
        FMortisSpawnEnemyRow* EnemyRow = EnemyRowHandle.GetRow<FMortisSpawnEnemyRow>(TEXT("SpawnEnemyRow"));
        if (!EnemyRow)
        {
            continue;
        }
        EnemiesToSpawn.Add(EnemyRow->EnemyClass);
    }
    
    const FTransform BaseTransform = GetSpawnTransform(Instigator, Target);
    const FVector BaseLocation = BaseTransform.GetLocation();

    for (const TSubclassOf<AMortisEnemyCharacter>& EnemyClass : EnemiesToSpawn)
    {
        if (!EnemyClass) continue;

        const FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.f, SpawnRadius);
        const FVector FinalLocation = BaseLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.f);
        
        SpawnWithIndicatorOrImmediate(Instigator->GetWorld(), EnemyClass, FinalLocation, Instigator->GetActorRotation(), DamageSpecHandle, Instigator);
    }
}