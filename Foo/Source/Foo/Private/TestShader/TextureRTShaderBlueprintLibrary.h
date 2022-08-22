// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TextureRTShaderBlueprintLibrary.generated.h"


UCLASS(MinimalAPI, meta=(ScriptName="LensDistortionLibrary"))
class UTextureRTShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	
	UFUNCTION(BlueprintCallable, Category = "Foo | Lens Distortion", meta = (WorldContext = "WorldContextObject"))
	static void DrawTextureToRenderTarget(
		const UObject* WorldContextObject,
		class UTextureRenderTarget2D* OutputRenderTarget,
		FLinearColor MyColor,
		UTexture* MyTexture
		);
};
