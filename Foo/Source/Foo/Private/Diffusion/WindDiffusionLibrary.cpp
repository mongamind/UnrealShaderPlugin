// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindDiffusionLibrary.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2


void UWindDiffusionLibrary::DrawWindDiffusion_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		const FWindSetting& WindSetting,
		FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer)
{
	check(IsInRenderingThread())

	TShaderMapRef<FWindOneAxisDiffusionShader> XAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(XAxisComputeShader.GetComputeShader());

	TShaderMapRef<FWindOneAxisDiffusionShader> YAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(YAxisComputeShader.GetComputeShader());

	TShaderMapRef<FWindOneAxisDiffusionShader> ZAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(ZAxisComputeShader.GetComputeShader());
	
	FComputeFenceRHIRef XAxisFence;
	FComputeFenceRHIRef YAxisFence;
	FComputeFenceRHIRef ZAxisFence;
	for(int i = 0;i < DIFFUSIONTIMES_EVERFRAME;++i)
	{
		WindVelocityTexturesDoubleBuffer->SwapBuffer();
		const FWindVelocityTextures& LastDiffusionTextures = WindVelocityTexturesDoubleBuffer->GetLastVelocityTextures();
		const FWindVelocityTextures& CurDiffusionTextures = WindVelocityTexturesDoubleBuffer->GetCurVelocityTextures();
		// DiffusionTextures.ClearUAVToBlack(RHICmdList);
		if(i != 0)
		{
			XAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::EMetaData, EResourceTransitionPipeline::EComputeToCompute, LastDiffusionTextures.WindDiffusionXAxisTexture_UAV, XAxisFence);
			XAxisComputeShader->UnbindBuffers(RHICmdList);
		}
		XAxisFence = RHICmdList.CreateComputeFence(TEXT("WindXAxisDiffusion"));
		XAxisComputeShader->SetParameters(RHICmdList,WindSetting,CurDiffusionTextures.WindDiffusionXAxisTexture_UAV,LastDiffusionTextures.WindDiffusionXAxisTexture_SRV);
		DispatchComputeShader(RHICmdList,XAxisComputeShader,1,1,1);
		// XAxisComputeShader->UnbindBuffers(RHICmdList);

		if(i != 0)
		{
			YAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::EMetaData, EResourceTransitionPipeline::EComputeToCompute, LastDiffusionTextures.WindDiffusionYAxisTexture_UAV, YAxisFence);
			YAxisComputeShader->UnbindBuffers(RHICmdList);
		}
		YAxisFence = RHICmdList.CreateComputeFence(TEXT("WindYAxisDiffusion"));
		YAxisComputeShader->SetParameters(RHICmdList,WindSetting,CurDiffusionTextures.WindDiffusionYAxisTexture_UAV,LastDiffusionTextures.WindDiffusionYAxisTexture_SRV);
		DispatchComputeShader(RHICmdList,YAxisComputeShader,1,1,1);
		// YAxisComputeShader->UnbindBuffers(RHICmdList);

		if(i != 0)
		{
			ZAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::EMetaData, EResourceTransitionPipeline::EComputeToCompute, LastDiffusionTextures.WindDiffusionZAxisTexture_UAV, ZAxisFence);
			YAxisComputeShader->UnbindBuffers(RHICmdList);
		}
		ZAxisFence = RHICmdList.CreateComputeFence(TEXT("WindZAxisDiffusion"));
		ZAxisComputeShader->SetParameters(RHICmdList,WindSetting,CurDiffusionTextures.WindDiffusionZAxisTexture_UAV,LastDiffusionTextures.WindDiffusionZAxisTexture_SRV);
		DispatchComputeShader(RHICmdList,ZAxisComputeShader,1,1,1);
		// ZAxisComputeShader->UnbindBuffers(RHICmdList);
	}

	const FWindVelocityTextures& LastDiffusionTextures = WindVelocityTexturesDoubleBuffer->GetCurVelocityTextures();
	LastDiffusionTextures.Barrier(RHICmdList,EResourceTransitionAccess::EMetaData,EResourceTransitionPipeline::EComputeToCompute);
	
}


void UWindDiffusionLibrary::DrawWindDiffusion(
	const UObject* WorldContextObject,
	FWindVelocityFieldDataParam &VelocityFieldData)
{
	check(IsInGameThread());
	
	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	FWindSetting WindSetting = VelocityFieldData.WindSetting;
	FWindVelocityTexturesDoubleBuffer* VelocityTexturesBuffer = VelocityFieldData.WindVelocityTexturesDoubleBuffer;

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,WindSetting,VelocityTexturesBuffer](FRHICommandListImmediate& RHICmdList)
		{
			DrawWindDiffusion_RenderThread(RHICmdList,FeatureLevel
											,WindSetting
											,VelocityTexturesBuffer);
		}
	);
	
	
}
