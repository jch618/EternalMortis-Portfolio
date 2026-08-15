// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/MortisEnemyData.h"

float UMortisEnemyScalingData::GetMultiplier(FName StatsName, int32 Floor) const
{
	if (!ScalingCurveTable)
	{
		return 1.f;
	}
	const FRealCurve* Curve = ScalingCurveTable->FindCurve(StatsName, TEXT("EnemyScaling"));
	if (!Curve)
	{
		return 1.f;
	}
	
	return Curve->Eval(static_cast<float>(Floor));
}
