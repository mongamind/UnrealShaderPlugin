// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UniformRTShaderBlueprintLibrary.generated.h"


USTRUCT(BlueprintType)
struct FMyShaderStructData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category=ShaderData)
	FLinearColor Color1;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category=ShaderData)
	FLinearColor Color2;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category=ShaderData)
	FLinearColor Color3;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category=ShaderData)
	FLinearColor Color4;
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category=ShaderData)
	int32 ColorIndex;
};

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FMyUniformStructData, )
	SHADER_PARAMETER(FVector4,Color1)
	SHADER_PARAMETER(FVector4,Color2)
	SHADER_PARAMETER(FVector4,Color3)
	SHADER_PARAMETER(FVector4,Color4)
	SHADER_PARAMETER(uint32,ColorIndex)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

//
// class FMyUniformStructData
// {
// 	DECLARE_INLINE_TYPE_LAYOUT(FMyUniformStructData, NonVirtual);
// public:
// 	void Bind(const FShaderParameterMap& ParameterMap)
// 	{
// 		Color1.Bind(ParameterMap, TEXT("Color1"));
// 		Color2.Bind(ParameterMap, TEXT("Color2"));
// 		Color3.Bind(ParameterMap, TEXT("Color3"));
// 		Color4.Bind(ParameterMap, TEXT("Color4"));
// 		ColorIndex.Bind(ParameterMap, TEXT("ColorIndex"));
// 	}
//
// 	bool IsBound() const
// 	{
// 		return true;
// 	}
//
// 	friend FArchive& operator<<(FArchive& Ar,FMyUniformStructData& Parameters)
// 	{
// 		Ar << Parameters.Color1;
// 		Ar << Parameters.Color2;
// 		Ar << Parameters.Color3;
// 		Ar << Parameters.Color4;
// 		Ar << Parameters.ColorIndex;
// 		return Ar;
// 	}
//
// 	template<typename ShaderRHIParamRef>
// 	FORCEINLINE_DEBUGGABLE void Set(FRHICommandList& RHICmdList, const ShaderRHIParamRef ShaderRHI, const FMyShaderStructData& ParameterData) const
// 	{
// 		if (IsBound())
// 		{
// 			SetShaderValue(RHICmdList, ShaderRHI, Color1, ParameterData.Color1);
// 			SetShaderValue(RHICmdList, ShaderRHI, Color2, ParameterData.Color2);
// 			SetShaderValue(RHICmdList, ShaderRHI, Color3, ParameterData.Color3);
// 			SetShaderValue(RHICmdList, ShaderRHI, Color4, ParameterData.Color4);
// 			SetShaderValue(RHICmdList, ShaderRHI, ColorIndex, ParameterData.ColorIndex);
// 		}
// 	}
//
// private:
// 	
// 	LAYOUT_FIELD(FShaderParameter, Color1)
// 	LAYOUT_FIELD(FShaderParameter, Color2)
// 	LAYOUT_FIELD(FShaderParameter, Color3)
// 	LAYOUT_FIELD(FShaderParameter, Color4)
// 	LAYOUT_FIELD(FShaderParameter, ColorIndex)
// };


UCLASS(MinimalAPI, meta=(ScriptName="LensDistortionLibrary"))
class UUniformRTShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	
	UFUNCTION(BlueprintCallable, Category = "WindSimulate", meta = (WorldContext = "WorldContextObject"))
	static void DrawUniformToRenderTarget(
		const UObject* WorldContextObject,
		class UTextureRenderTarget2D* OutputRenderTarget,
		FLinearColor MyColor,
		UTexture* MyTexture,
		const FMyShaderStructData& UniformStructData
		);
};
