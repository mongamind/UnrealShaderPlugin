// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Debug/WindDebugData.h"
#include "WindDebugLibrary.generated.h"



UCLASS(MinimalAPI, meta=(ScriptName="Wind Simulation Library"))
class UWindDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// UFUNCTION(BlueprintCallable, Category = "Wind Simulate | Debug", meta = (WorldContext = "WorldContextObject"))
	static void DrawWindXYZAxisToRT(
				const UObject* WorldContextObject,
				float Padding,
				float MaxWindVelocity,
				FWindVelocityTextures* WindTextureBuffers,
				class UTextureRenderTarget2D* OutputRenderTarget);

	static void DrawWindTex3DToRT(
			const UObject* WorldContextObject,
			float Padding,
			float MaxWindVelocity,
			FWindVelocityTextures* WindTextureBuffers,
			class UTextureRenderTarget2D* OutputRenderTarget);

private:
	static void DrawWindXYZAxisToRT_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FTextureRenderTargetResource* OutTextureRenderTargetResource,
		float Padding,
		float MaxWindVelocity,
		FShaderResourceViewRHIRef TextureXAxisRHI,
		FShaderResourceViewRHIRef TextureYAxisRHI,
		FShaderResourceViewRHIRef TextureZAxisRHI);

	static void DrawWindTex3DToRT_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FTextureRenderTargetResource* OutTextureRenderTargetResource,
		float Padding,
		float MaxWindVelocity,
		FShaderResourceViewRHIRef TextureXAxisRHI,
		FShaderResourceViewRHIRef TextureYAxisRHI,
		FShaderResourceViewRHIRef TextureZAxisRHI);
};


