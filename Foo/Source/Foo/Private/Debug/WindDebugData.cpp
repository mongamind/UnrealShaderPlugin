// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/WindDebugData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ClearQuad.h"


IMPLEMENT_SHADER_TYPE(, FWindDebugShaderVS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FWindDebugShaderPS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainPS"), SF_Pixel)

// IMPLEMENT_SHADER_TYPE(, FWindDebugShaderVS, TEXT("/Plugin/Foo/Private/MyColorShader.usf"), TEXT("MainVS"), SF_Vertex)
// IMPLEMENT_SHADER_TYPE(, FWindDebugShaderPS, TEXT("/Plugin/Foo/Private/MyColorShader.usf"), TEXT("MainPS"), SF_Pixel)

FWindDebugShaderBase::FWindDebugShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
    MyColor.Bind(Initializer.ParameterMap,TEXT("MyColor"));
	// Padding.Bind(Initializer.ParameterMap,TEXT("Padding"));
	//
	// WindDiffusionXAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionXAxisTexture"));
	// WindDiffusionYAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionYAxisTexture"));
	// WindDiffusionZAxisTexture.Bind(Initializer.ParameterMap,TEXT("WindDiffusionZAxisTexture"));
}

void FWindDebugShaderBase::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	// OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FWindDebugShaderBase::SetParameters(
	FRHICommandListImmediate& RHICmdList,
	float InPadding,
	FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
	FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
	FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV)
{

	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();

	SetShaderValue(RHICmdList,ComputeShaderRHI,MyColor,FLinearColor(1,0,0,1));
	// SetShaderValue(RHICmdList,ComputeShaderRHI,Padding,InPadding);
	//
	// SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionXAxisTexture,WindDiffusionXAxisTexture_SRV);
	// SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionYAxisTexture,WindDiffusionYAxisTexture_SRV);
	// SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionZAxisTexture,WindDiffusionZAxisTexture_SRV);
}

void FWindDebugShaderBase::UnbindBuffers(FRHICommandListImmediate& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();

	// if(WindDiffusionXAxisTexture.IsBound())
	// {
	// 	SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionXAxisTexture,nullptr);
	// 	SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionYAxisTexture,nullptr);
	// 	SetSRVParameter(RHICmdList,ComputeShaderRHI,WindDiffusionZAxisTexture,nullptr);
	// }
}