// Copyright Epic Games, Inc. All Rights Reserved.

#include "SphereMotorData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ClearQuad.h"


IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FSphereMotorUniformData, "SphereMotorData");

FSphereMotorShader::FSphereMotorShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	InXAxisTexture.Bind(Initializer.ParameterMap,TEXT("InXAxisTexture"));
	InYAxisTexture.Bind(Initializer.ParameterMap,TEXT("InYAxisTexture"));
	InZAxisTexture.Bind(Initializer.ParameterMap,TEXT("InZAxisTexture"));

	OutXAxisTexture.Bind(Initializer.ParameterMap,TEXT("OutXAxisTexture"));
	OutYAxisTexture.Bind(Initializer.ParameterMap,TEXT("OutYAxisTexture"));
	OutZAxisTexture.Bind(Initializer.ParameterMap,TEXT("OutZAxisTexture"));
}

void FSphereMotorShader::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FSphereMotorShader::SetParameters(
	FRHICommandListImmediate& RHICmdList,
	FVector WindCenterWorldPos,
	float MaxVelocity,
	FVector MetersPerTexel,
	const TArray<FSphereMotorParam*>& AllWindMotors,
	FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
	FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
	FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
	FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
	FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
	FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV)
{

	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if(OutXAxisTexture.IsBound())
	{
		SetSRVParameter(RHICmdList,ComputeShaderRHI,InXAxisTexture,WindDiffusionXAxisTexture_SRV);
		SetSRVParameter(RHICmdList,ComputeShaderRHI,InYAxisTexture,WindDiffusionYAxisTexture_SRV);
		SetSRVParameter(RHICmdList,ComputeShaderRHI,InZAxisTexture,WindDiffusionZAxisTexture_SRV);

		TArray<FVector,TInlineAllocator<64>> MotorPosArray;
		TArray<FVector,TInlineAllocator<64>> MotorWindVelocityArray;
		TArray<float,TInlineAllocator<64>> MotorRadiusArray;

		// MotorPosArray.Empty(64);
		// MotorWindVelocityArray.Empty(64);
		// MotorRadiusArray.Empty(64);
		FSphereMotorUniformData UniformData;
		for (int index = 0;index < AllWindMotors.Num();index ++)
		{
			const FSphereMotorParam* MotorParam = AllWindMotors[index];
			UniformData.MotorWorldSpacePos[index] = MotorParam->MotorWorldSpacePos;
			UniformData.MotorWindForce[index] = MotorParam->WindForce;
			UniformData.MotorRadius[index] = MotorParam->Radius;
		}
		
		UniformData.NumMotors = AllWindMotors.Num();
		UniformData.MaxVelocity = MaxVelocity;
		UniformData.MetersPerTexel = MetersPerTexel;
		UniformData.InWindCenterWorldPos = WindCenterWorldPos;
		SetUniformBufferParameterImmediate(RHICmdList,ComputeShaderRHI,GetUniformBufferParameter<FSphereMotorUniformData>(), UniformData);
		
		// SetShaderValue(RHICmdList,ComputeShaderRHI,NumMotors,AllWindMotors.Num());
		// SetShaderValue(RHICmdList,ComputeShaderRHI,InWindCenterWorldPos,PlayerWorldPos);
		// SetShaderValueArray(RHICmdList,ComputeShaderRHI,MotorWorldSpacePos,MotorPosArray.GetData(),AllWindMotors.Num());
		// SetShaderValueArray(RHICmdList,ComputeShaderRHI,MotorWindVelocity,MotorWindVelocityArray.GetData(),AllWindMotors.Num());
		// SetShaderValueArray(RHICmdList,ComputeShaderRHI,MotorRadius,MotorRadiusArray.GetData(),AllWindMotors.Num());
		

		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutXAxisTexture,WindDiffusionXAxisTexture_UAV);
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutYAxisTexture,WindDiffusionYAxisTexture_UAV);
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutZAxisTexture,WindDiffusionZAxisTexture_UAV);

	}
}

void FSphereMotorShader::UnbindBuffers(FRHICommandListImmediate& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if(OutXAxisTexture.IsBound())
	{
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutXAxisTexture,nullptr);
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutYAxisTexture,nullptr);
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutZAxisTexture,nullptr);
	}
}

void FSphereMotorShader::UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
		FRHIComputeFence* Fence)
{
	FRHIComputeShader* ShaderRHI = RHICmdList.GetBoundComputeShader();

	SetUAVParameter(RHICmdList, ShaderRHI, OutXAxisTexture, FUnorderedAccessViewRHIRef());
	SetUAVParameter(RHICmdList, ShaderRHI, OutYAxisTexture, FUnorderedAccessViewRHIRef());
	SetUAVParameter(RHICmdList, ShaderRHI, OutZAxisTexture, FUnorderedAccessViewRHIRef());
	FRHIUnorderedAccessView* UAVs[] = {
		WindDiffusionXAxisTexture_UAV,
		WindDiffusionYAxisTexture_UAV,
		WindDiffusionZAxisTexture_UAV
	};
	RHICmdList.TransitionResources(TransitionAccess, TransitionPipeline, UAVs, 3, Fence);
}

IMPLEMENT_SHADER_TYPE(, FSphereMotorShader, TEXT("/Plugin/WindSimulate/Private/ComputeShader/Motor/SphereMotor.usf"), TEXT("MainCS"), SF_Compute)

