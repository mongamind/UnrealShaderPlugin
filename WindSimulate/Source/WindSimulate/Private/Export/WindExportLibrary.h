// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Export/ExportData.h"
#include "WindExportLibrary.generated.h"


UCLASS(MinimalAPI, meta=(ScriptName="Wind Simulation Library"))
class UWindExportLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void ExportWindTextures(
		const UObject* WorldContextObject,
		struct FWindVelocityTextures* WindVelocityTextures,
		class UTextureRenderTarget2D* OutputRenderTarget/*,
		float InMaxWindVelocity*/
		);

private:
	static void ExportWindTextures_RenderThread(FRHICommandListImmediate& RHICmdList,
			ERHIFeatureLevel::Type FeatureLevel,
			FShaderResourceViewRHIRef TextureXAxisRHI,
			FShaderResourceViewRHIRef TextureYAxisRHI,
			FShaderResourceViewRHIRef TextureZAxisRHI,
			FTextureRenderTargetResource* OutTextureRenderTargetResource/*,
			float InMaxWindVelocity*/);
};


