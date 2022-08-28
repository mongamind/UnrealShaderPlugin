// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Diffusion/WindDiffusionData.h"
#include "WindMotorData.h"

class FSphereMotorParam : public FWindMotorBaseParamBase
{
public:
	virtual ~FSphereMotorParam() override{}
	virtual EWindMotorType GetWindMotorType() const override {return WMT_Sphere;};
};


BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FSphereMotorUniformData, )
	SHADER_PARAMETER(uint32,NumMotors)
	SHADER_PARAMETER(float,MaxVelocity)
	SHADER_PARAMETER(FVector,MetersPerTexel)
	SHADER_PARAMETER(FVector,InPlayerWorldSpacePos)
	SHADER_PARAMETER_ARRAY(float,MotorRadius, [64])
	SHADER_PARAMETER_ARRAY(FVector,MotorWorldSpacePos, [64])
	SHADER_PARAMETER_ARRAY(FVector,MotorWindVelocity, [64])
END_GLOBAL_SHADER_PARAMETER_STRUCT()

class FSphereMotorShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSphereMotorShader, Global);
public:

	FSphereMotorShader() {}

	FSphereMotorShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
	
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		FVector PlayerWorldPos,
		float MaxVelocity,
		FVector MetersPerTexel,
		const TArray<FSphereMotorParam*>& AllWindMotors,
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
	
	LAYOUT_FIELD(FShaderResourceParameter, OutXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutZAxisTexture);
};
