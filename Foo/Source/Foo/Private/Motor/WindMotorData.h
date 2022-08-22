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
	FVector PlayerWorldPos;
	float MaxVelocity;
	float TexelsPerMeter;
	FWindVelocityTextures* WindVelocityTexturesDoubleBuffer;
};
//
// BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FDirectionalWindMotorData, )
// 	SHADER_PARAMETER(FVector,MotorWorldSpacePos)
// 	SHADER_PARAMETER(FVector,WindVelocity)
// 	SHADER_PARAMETER(float,radius)
// END_GLOBAL_SHADER_PARAMETER_STRUCT()



BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FMotorUniformData, )
	SHADER_PARAMETER(uint32,NumMotors)
	SHADER_PARAMETER(float,MaxVelocity)
	SHADER_PARAMETER(float,TexelsPerMeter)
	SHADER_PARAMETER(FVector,InPlayerWorldSpacePos)
	SHADER_PARAMETER_ARRAY(float,MotorRadius, [64])
	SHADER_PARAMETER_ARRAY(FVector,MotorWorldSpacePos, [64])
	SHADER_PARAMETER_ARRAY(FVector,MotorWindVelocity, [64])
END_GLOBAL_SHADER_PARAMETER_STRUCT()

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
		FVector PlayerWorldPos,
		float MaxVelocity,
		float TexelsPermeter,
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

	// LAYOUT_FIELD(FShaderParameter, InPlayerWorldSpacePos);
	//
	// LAYOUT_FIELD(FShaderParameter, MotorWorldSpacePos);
	// LAYOUT_FIELD(FShaderParameter, MotorWindVelocity);
	// LAYOUT_FIELD(FShaderParameter, MotorRadius);
	
	LAYOUT_FIELD(FShaderResourceParameter, OutXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutZAxisTexture);
};
