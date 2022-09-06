// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindDiffusionData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ClearQuad.h"


IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FWindVelocityFieldData, "WindVelocityData");

void FWindVelocityTextures::Init(EPixelFormat InFormat, uint32 InX, uint32 InY, uint32 InZ)
{
	FRHIResourceCreateInfo CreateInfo;

	for(int i = 0;i < 2;++i)
	{
		WindDiffusionXAxisTexture[i] = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		WindDiffusionXAxisTexture_UAV[i] = RHICreateUnorderedAccessView(WindDiffusionXAxisTexture[i]);
		WindDiffusionXAxisTexture_SRV[i] = RHICreateShaderResourceView(WindDiffusionXAxisTexture[i], 0);

		WindDiffusionYAxisTexture[i] = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		WindDiffusionYAxisTexture_UAV[i] = RHICreateUnorderedAccessView(WindDiffusionYAxisTexture[i]);
		WindDiffusionYAxisTexture_SRV[i] = RHICreateShaderResourceView(WindDiffusionYAxisTexture[i], 0);

		WindDiffusionZAxisTexture[i] = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		WindDiffusionZAxisTexture_UAV[i] = RHICreateUnorderedAccessView(WindDiffusionZAxisTexture[i]);
		WindDiffusionZAxisTexture_SRV[i] = RHICreateShaderResourceView(WindDiffusionZAxisTexture[i], 0);
	}


	SizeX = InX;
	SizeY = InY;
	SizeZ = InZ;
}

void FWindVelocityTextures::Release()
{
	for(int i = 0;i < 2;++i)
	{
		WindDiffusionXAxisTexture[i].SafeRelease();
		WindDiffusionXAxisTexture_UAV[i].SafeRelease();
		WindDiffusionXAxisTexture_SRV[i].SafeRelease();

		WindDiffusionYAxisTexture[i].SafeRelease();
		WindDiffusionYAxisTexture_UAV[i].SafeRelease();
		WindDiffusionYAxisTexture_SRV[i].SafeRelease();

		WindDiffusionZAxisTexture[i].SafeRelease();
		WindDiffusionZAxisTexture_UAV[i].SafeRelease();
		WindDiffusionZAxisTexture_SRV[i].SafeRelease();
	}

	
	SizeX = 0;
	SizeY = 0;
	SizeZ = 0;
}

void FWindVelocityTextures::ClearCurXAxisUAVToBlack(FRHICommandList& RHICmdList) const
{
	RHICmdList.ClearUAVFloat(GetCurXAxisUAV(),FVector4(0,0,0,0));
}

void FWindVelocityTextures::ClearCurYAxisUAVToBlack(FRHICommandList& RHICmdList) const
{
	RHICmdList.ClearUAVFloat(GetCurYAxisUAV(),FVector4(0,0,0,0));
}

void FWindVelocityTextures::ClearCurZAxisUAVToBlack(FRHICommandList& RHICmdList) const
{
	RHICmdList.ClearUAVFloat(GetCurZAxisUAV(),FVector4(0,0,0,0));
}

void FWindVelocityTextures::Barrier(FRHICommandList& RHICmdList, EResourceTransitionAccess InBarrierMode, EResourceTransitionPipeline InPipeline,FRHIComputeFence* Fence) const
{
	FRHIUnorderedAccessView* UAVs[] = {
		GetCurXAxisUAV(),
		GetCurYAxisUAV(),
		GetCurZAxisUAV()
	};
	RHICmdList.TransitionResources(InBarrierMode, InPipeline, UAVs,3, Fence);
}



// void FWindVelocityTexturesDoubleBuffer::InitVelocityTextures()
// {
// 	for (int index = 0;index < 2;++index)
// 	{
// 		VelocityTextures[index].Init(EPixelFormat::PF_R32_FLOAT,32,32,16);
// 	}
// }
//
// void FWindVelocityTexturesDoubleBuffer::ReleaseVelocityTextures()
// {
// 	for (int index = 0;index < 2;++index)
// 	{
// 		VelocityTextures[index].Release();
// 	}
// }




FWindOneAxisDiffusionShader::FWindOneAxisDiffusionShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	InTexture.Bind(Initializer.ParameterMap,TEXT("InTexture"));
	OutTexture.Bind(Initializer.ParameterMap,TEXT("OutTexture"));
}

void FWindOneAxisDiffusionShader::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FWindOneAxisDiffusionShader::SetParameters(
	FRHICommandListImmediate& RHICmdList,
	const FWindSetting& WindSetting,
	FUnorderedAccessViewRHIRef WindDiffusionOneAxisTexture_UAV,
	FShaderResourceViewRHIRef WindDiffusionOneAxisTexture_SRV)
{

	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if(OutTexture.IsBound())
	{
		SetSRVParameter(RHICmdList,ComputeShaderRHI,InTexture,WindDiffusionOneAxisTexture_SRV);

		FWindVelocityFieldData UniformData;
		UniformData.InMaxWindVelocity = WindSetting.MaxWindVelocity;
		UniformData.MetersPerTexel = WindSetting.MetersPerTexel;
		UniformData.InPosDelta = WindSetting.InPosDelta;
		UniformData.OriginalAlpha = WindSetting.OriginalAlpha;
		UniformData.BetaNearAdd = WindSetting.BetaNearAdd;
		SetUniformBufferParameterImmediate(RHICmdList,ComputeShaderRHI,GetUniformBufferParameter<FWindVelocityFieldData>(), UniformData);

		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutTexture,WindDiffusionOneAxisTexture_UAV);
	}
}

void FWindOneAxisDiffusionShader::UnbindBuffers(FRHICommandListImmediate& RHICmdList)
{
	FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
	if(OutTexture.IsBound())
	{
		SetSRVParameter(RHICmdList,ComputeShaderRHI,InTexture,nullptr);
		SetUAVParameter(RHICmdList,ComputeShaderRHI,OutTexture,nullptr);
	}
}

void FWindOneAxisDiffusionShader::UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionOneAxisTexture_UAV,
		FRHIComputeFence* Fence)
{
	FRHIComputeShader* ShaderRHI = RHICmdList.GetBoundComputeShader();

	SetUAVParameter(RHICmdList, ShaderRHI, OutTexture, FUnorderedAccessViewRHIRef());
	RHICmdList.TransitionResource(TransitionAccess, TransitionPipeline, WindDiffusionOneAxisTexture_UAV, Fence);
	
}

void FWindOneAxisDiffusionShader::UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
		FRHIComputeFence* Fence)
{
	FRHIComputeShader* ShaderRHI = RHICmdList.GetBoundComputeShader();

	SetUAVParameter(RHICmdList, ShaderRHI, OutTexture, FUnorderedAccessViewRHIRef());
	FRHIUnorderedAccessView* UAVs[] = {
		WindDiffusionXAxisTexture_UAV,
		WindDiffusionYAxisTexture_UAV,
		WindDiffusionZAxisTexture_UAV
	};
	RHICmdList.TransitionResources(TransitionAccess, TransitionPipeline, UAVs,3, Fence);
}

IMPLEMENT_SHADER_TYPE(, FWindOneAxisDiffusionShader, TEXT("/Plugin/WindSimulate/Private/ComputeShader/Diffusion/WindOneAxisDiffusion.usf"), TEXT("MainCS"), SF_Compute)

