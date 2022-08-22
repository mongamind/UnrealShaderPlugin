// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindDiffusionLibrary.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2


void UWindDiffusionLibrary::DrawWindDiffusion_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		const FWindSetting& WindSetting,
		FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
{
	check(IsInRenderingThread())

	TShaderMapRef<FWindOneAxisDiffusionShader> XAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(XAxisComputeShader.GetComputeShader());

	TShaderMapRef<FWindOneAxisDiffusionShader> YAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(YAxisComputeShader.GetComputeShader());

	TShaderMapRef<FWindOneAxisDiffusionShader> ZAxisComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(ZAxisComputeShader.GetComputeShader());
	
	FComputeFenceRHIRef XAxisFence = RHICmdList.CreateComputeFence(TEXT("WindXAxisFence"));
	FComputeFenceRHIRef YAxisFence = RHICmdList.CreateComputeFence(TEXT("WindYAxisFence"));
	FComputeFenceRHIRef ZAxisFence = RHICmdList.CreateComputeFence(TEXT("WindZAxisFence"));
	for(int i = 0;i < DIFFUSIONTIMES_EVERFRAME;++i)
	{
		if(i != 0)
		{
			XAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute, WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(), XAxisFence);
			RHICmdList.WaitComputeFence(XAxisFence);
			WindVelocityTexturesDoubleBuffer->SwapXAxisBuff();
		}
		// WindVelocityTexturesDoubleBuffer->ClearCurXAxisUAVToBlack(RHICmdList);  it will turn back if call this.
		XAxisComputeShader->SetParameters(RHICmdList,WindSetting,WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),WindVelocityTexturesDoubleBuffer->GetCurXAxisSRV());
		DispatchComputeShader(RHICmdList,XAxisComputeShader,1,1,1);

		if(i != 0)
		{
			YAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute, WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(), YAxisFence);
			RHICmdList.WaitComputeFence(YAxisFence);
			WindVelocityTexturesDoubleBuffer->SwapYAxisBuff();
		}
		// WindVelocityTexturesDoubleBuffer->ClearCurYAxisUAVToBlack(RHICmdList);	it will turn back if call this.
		YAxisComputeShader->SetParameters(RHICmdList,WindSetting,WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),WindVelocityTexturesDoubleBuffer->GetCurYAxisSRV());
		DispatchComputeShader(RHICmdList,YAxisComputeShader,1,1,1);

		if(i != 0)
		{
			ZAxisComputeShader->UnsetParameters(RHICmdList, EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute,  WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(), ZAxisFence);
			RHICmdList.WaitComputeFence(ZAxisFence);
			WindVelocityTexturesDoubleBuffer->SwapZAxisBuff();
		}
		// WindVelocityTexturesDoubleBuffer->ClearCurZAxisUAVToBlack(RHICmdList);	it will turn back if call this.
		ZAxisComputeShader->SetParameters(RHICmdList,WindSetting,WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),WindVelocityTexturesDoubleBuffer->GetCurZAxisSRV());
		DispatchComputeShader(RHICmdList,ZAxisComputeShader,1,1,1);
	}

	FComputeFenceRHIRef AllAxisFence = RHICmdList.CreateComputeFence(TEXT("WindAllAxisFence"));
	WindVelocityTexturesDoubleBuffer->Barrier(RHICmdList,EResourceTransitionAccess::ERWBarrier,EResourceTransitionPipeline::EComputeToCompute,AllAxisFence);
	WindVelocityTexturesDoubleBuffer->SwapXAxisBuff();
	WindVelocityTexturesDoubleBuffer->SwapYAxisBuff();
	WindVelocityTexturesDoubleBuffer->SwapZAxisBuff();
}


void UWindDiffusionLibrary::DrawWindDiffusion(
	const UObject* WorldContextObject,
	FWindVelocityFieldDataParam &VelocityFieldData)
{
	check(IsInGameThread());
	
	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	FWindSetting WindSetting = VelocityFieldData.WindSetting;
	FWindVelocityTextures* VelocityTexturesBuffer = VelocityFieldData.WindVelocityTexturesDoubleBuffer;

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,WindSetting,VelocityTexturesBuffer](FRHICommandListImmediate& RHICmdList)
		{
			DrawWindDiffusion_RenderThread(RHICmdList,FeatureLevel
											,WindSetting
											,VelocityTexturesBuffer);
		}
	);
	
	
}
