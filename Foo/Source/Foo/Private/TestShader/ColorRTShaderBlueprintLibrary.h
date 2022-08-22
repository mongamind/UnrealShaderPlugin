// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorRTShaderBlueprintLibrary.generated.h"


UCLASS(MinimalAPI, meta=(ScriptName="LensDistortionLibrary"))
class UColorRTShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	
	UFUNCTION(BlueprintCallable, Category = "Foo | Lens Distortion", meta = (WorldContext = "WorldContextObject"))
	static void DrawSimpleColorToRenderTarget(
		const UObject* WorldContextObject,
		class UTextureRenderTarget2D* OutputRenderTarget,
		FLinearColor MyColor
		);
};
