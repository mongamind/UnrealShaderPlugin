// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindMotorData.h"
#include "WindMotorLibrary.generated.h"


UCLASS(MinimalAPI, meta=(ScriptName="Wind Simulation Library"))
class UWindMotorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void ApplyWindMotors(
		const UObject* WorldContextObject,
		FWindMotorParamData &WindMotordData);

private:
	static void ApplyWindMotors_RenderThread(
					FRHICommandListImmediate& RHICmdList,
					ERHIFeatureLevel::Type FeatureLevel,
					const TArray<FWindMotorBaseParamBase*>& AllWindMotors,
					FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer);
};


