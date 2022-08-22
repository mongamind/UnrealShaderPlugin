// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/WindDebugData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ClearQuad.h"


IMPLEMENT_SHADER_TYPE(, FWindDebugShaderVS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FWindDebugShaderPS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainPS"), SF_Pixel)

// IMPLEMENT_SHADER_TYPE(, FWindDebugShaderVS, TEXT("/Plugin/Foo/Private/TestShader/MyColorShader.usf"), TEXT("MainVS"), SF_Vertex)
// IMPLEMENT_SHADER_TYPE(, FWindDebugShaderPS, TEXT("/Plugin/Foo/Private/TestShader/MyColorShader.usf"), TEXT("MainPS"), SF_Pixel)

FWindDebugShaderBase::FWindDebugShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	Padding.Bind(Initializer.ParameterMap,TEXT("Padding"));
	MaxWindVelocity.Bind(Initializer.ParameterMap,TEXT("MaxWindVelocity"));
	
	WindDiffusionXAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionXAxisTexture"));
	WindDiffusionYAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionYAxisTexture"));
	WindDiffusionZAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionZAxisTexture"));
}

void FWindDebugShaderBase::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	// OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

template<typename TShaderRHIParamRef>
void FWindDebugShaderBase::SetParameters(
	FRHICommandListImmediate& RHICmdList,
	const TShaderRHIParamRef ShaderRHI,
	float InPadding,
	float InMaxWindVelocity,
	FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
	FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
	FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV)
{
	SetShaderValue(RHICmdList,ShaderRHI,Padding,InPadding);
	SetShaderValue(RHICmdList,ShaderRHI,MaxWindVelocity,InMaxWindVelocity);
	
	SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionXAxisTexture,WindDiffusionXAxisTexture_SRV);
	SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionYAxisTexture,WindDiffusionYAxisTexture_SRV);
	SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionZAxisTexture,WindDiffusionZAxisTexture_SRV);
}

void FWindDebugShaderBase::UnbindBuffers(FRHICommandListImmediate& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();

	if(WindDiffusionXAxisTexture.IsBound())
	{
		SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionXAxisTexture,nullptr);
		SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionYAxisTexture,nullptr);
		SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionZAxisTexture,nullptr);
	}
}