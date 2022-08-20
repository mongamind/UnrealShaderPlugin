// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Diffusion/WindDiffusionData.h"

enum EWindMotorType
{
	WMT_Unknown,
	WMT_Directional,						//
};

class FWindMotorBaseParamBase
{
public:
	virtual ~FWindMotorBaseParamBase(){}
	virtual EWindMotorType GetWindMotorType() const{return WMT_Unknown;};

public:
	FVector MotorWorldSpacePos;                  //Min of wind velocity
	FVector WindVelocity;
	float MaxVelocity;		
	float Radius = 1;								//wind drag force
};

class FDirectionalWindMotorParam : public FWindMotorBaseParamBase
{
public:
	virtual ~FDirectionalWindMotorParam() override{}
	virtual EWindMotorType GetWindMotorType() const override {return WMT_Directional;};
};

struct FWindMotorParamData
{
	TArray<FWindMotorBaseParamBase*> AllWindMotors;

	FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer;
};
//
// BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FDirectionalWindMotorData, )
// 	SHADER_PARAMETER(FVector,MotorWorldSpacePos)
// 	SHADER_PARAMETER(FVector,WindVelocity)
// 	SHADER_PARAMETER(float,radius)
// END_GLOBAL_SHADER_PARAMETER_STRUCT()

class FWindDirectionalMotorShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FWindDirectionalMotorShader, Global);
public:

	FWindDirectionalMotorShader() {}

	FWindDirectionalMotorShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
	
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TArray<FDirectionalWindMotorParam>& AllWindMotors,
		FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
		FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
		FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
		FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
		FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
		FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV);

	void UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
		FRHIComputeFence* Fence);

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList);

private:
	LAYOUT_FIELD(FShaderResourceParameter, InXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InZAxisTexture);

	LAYOUT_FIELD(FShaderParameter, InPlayerWorldSpacePos);

	LAYOUT_FIELD(FShaderParameter, NumMotors);
	LAYOUT_FIELD(FShaderParameter, MotorWorldSpacePos);
	LAYOUT_FIELD(FShaderParameter, MotorWindVelocity);
	LAYOUT_FIELD(FShaderParameter, MotorRadius);
	
	LAYOUT_FIELD(FShaderResourceParameter, OutXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutZAxisTexture);
};
