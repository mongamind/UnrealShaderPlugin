// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindDiffusionData.h"
#include "WindDiffusionLibrary.generated.h"


UCLASS(MinimalAPI, meta=(ScriptName="Wind Simulation Library"))
class UWindDiffusionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void DrawWindDiffusion(
		const UObject* WorldContextObject,
		FWindVelocityFieldDataParam &VelocityFieldData);

private:
	static void DrawWindDiffusion_RenderThread(
					FRHICommandListImmediate& RHICmdList,
					ERHIFeatureLevel::Type FeatureLevel,
					const FWindSetting& WindSetting,
					FWindVelocityTextures* WindVelocityTexturesDoubleBuffer);
};


