// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindDiffusionData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ClearQuad.h"


IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FWindVelocityFieldData, "WindVelocityData");

void FWindVelocityTextures::Init(EPixelFormat InFormat, uint32 InX, uint32 InY, uint32 InZ)
{
	FRHIResourceCreateInfo CreateInfo;
	
	WindDiffusionXAxisTexture = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	WindDiffusionXAxisTexture_UAV = RHICreateUnorderedAccessView(WindDiffusionXAxisTexture);
	WindDiffusionXAxisTexture_SRV = RHICreateShaderResourceView(WindDiffusionXAxisTexture, 0);

	WindDiffusionYAxisTexture = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	WindDiffusionYAxisTexture_UAV = RHICreateUnorderedAccessView(WindDiffusionYAxisTexture);
	WindDiffusionYAxisTexture_SRV = RHICreateShaderResourceView(WindDiffusionYAxisTexture, 0);

	WindDiffusionZAxisTexture = RHICreateTexture3D(InX, InY, InZ, InFormat, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	WindDiffusionZAxisTexture_UAV = RHICreateUnorderedAccessView(WindDiffusionZAxisTexture);
	WindDiffusionZAxisTexture_SRV = RHICreateShaderResourceView(WindDiffusionZAxisTexture, 0);

	SizeX = InX;
	SizeY = InY;
	SizeZ = InZ;
}

void FWindVelocityTextures::Release()
{
	WindDiffusionXAxisTexture.SafeRelease();
	WindDiffusionXAxisTexture_UAV.SafeRelease();
	WindDiffusionXAxisTexture_SRV.SafeRelease();

	WindDiffusionYAxisTexture.SafeRelease();
	WindDiffusionYAxisTexture_UAV.SafeRelease();
	WindDiffusionYAxisTexture_SRV.SafeRelease();

	WindDiffusionZAxisTexture.SafeRelease();
	WindDiffusionZAxisTexture_UAV.SafeRelease();
	WindDiffusionZAxisTexture_SRV.SafeRelease();
	
	SizeX = 0;
	SizeY = 0;
	SizeZ = 0;
}

void FWindVelocityTextures::ClearUAVToBlack(FRHICommandList& RHICmdList) const
{
	RHICmdList.ClearUAVFloat(WindDiffusionXAxisTexture_UAV,FVector4(0,0,0,0));
	RHICmdList.ClearUAVFloat(WindDiffusionYAxisTexture_UAV,FVector4(0,0,0,0));
	RHICmdList.ClearUAVFloat(WindDiffusionZAxisTexture_UAV,FVector4(0,0,0,0));
}

void FWindVelocityTextures::Barrier(FRHICommandList& RHICmdList, EResourceTransitionAccess InBarrierMode, EResourceTransitionPipeline InPipeline) const
{
	RHICmdList.TransitionResource(InBarrierMode, InPipeline, WindDiffusionXAxisTexture_UAV);
	RHICmdList.TransitionResource(InBarrierMode, InPipeline, WindDiffusionYAxisTexture_UAV);
	RHICmdList.TransitionResource(InBarrierMode, InPipeline, WindDiffusionZAxisTexture_UAV);
}



void FWindVelocityTexturesDoubleBuffer::InitVelocityTextures()
{
	for (int index = 0;index < 2;++index)
	{
		VelocityTextures[index].Init(EPixelFormat::PF_R32_FLOAT,32,32,16);
	}
}

void FWindVelocityTexturesDoubleBuffer::ReleaseVelocityTextures()
{
	for (int index = 0;index < 2;++index)
	{
		VelocityTextures[index].Release();
	}
}



uint32 FWindDiffuse2DArrayResource::GetSizeX() const
{
	return SizeX;
}

uint32 FWindDiffuse2DArrayResource::GetSizeY() const
{
	return SizeY;
}

uint32 FWindDiffuse2DArrayResource::GetSizeZ() const
{
	return SizeZ;
}

void FWindDiffuse2DArrayResource::InitRHI()
{
	FTextureResource::InitRHI();

	FRHIResourceCreateInfo CreateInfo;
	uint32 Flags = TexCreate_NoTiling | TexCreate_OfflineProcessed;

	if (CreateUAV)
	{
		Flags |= TexCreate_UAV;
	}

	TextureRHI = RHICreateTexture2DArray(SizeX, SizeY, SizeZ, Format, NumMips, 1, Flags, CreateInfo);

	if (CreateUAV)
	{
		TextureUAV = RHICreateUnorderedAccessView(TextureRHI, 0);
	}
}

void FWindDiffuse2DArrayResource::ReleaseRHI()
{
	FTextureResource::ReleaseRHI();

	TextureUAV.SafeRelease();
}


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
		UniformData.InMaxWindVelocity = WindSetting.InMaxWindVelocity;
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
	FRHIUnorderedAccessView* UAVs[] = {
		WindDiffusionOneAxisTexture_UAV
	};
	RHICmdList.TransitionResources(TransitionAccess, TransitionPipeline, UAVs, 1, Fence);
}

IMPLEMENT_SHADER_TYPE(, FWindOneAxisDiffusionShader, TEXT("/Plugin/Foo/Private/ComputeShader/Diffusion/WindOneAxisDiffusion.usf"), TEXT("MainCS"), SF_Compute)

