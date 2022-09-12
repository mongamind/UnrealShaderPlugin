// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindExportLibrary.h"
#include "Diffusion/WindDiffusionData.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2

TGlobalResource<FWindExportVertexBuffer> g_WindExportVertexBuffer;
TGlobalResource<FWindExportIndexBuffer> g_WindExportIndexBuffer;

void UWindExportLibrary::ExportWindTextures_RenderThread(FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FShaderResourceViewRHIRef TextureXAxisRHI,
		FShaderResourceViewRHIRef TextureYAxisRHI,
		FShaderResourceViewRHIRef TextureZAxisRHI,
		FTextureRenderTargetResource* OutTextureRenderTargetResource/*,
		float InMaxWindVelocity*/)
{
		check(IsInRenderingThread());

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();
	RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, RenderTargetTexture);

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store, OutTextureRenderTargetResource->TextureRHI);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("WindExportShaderPass"));
	{
		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef< FWindExportShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FWindExportShaderPS > PixelShader(GlobalShaderMap);

		FWindExportVertexDeclaration vertexDeclartion;
		vertexDeclartion.InitRHI();

		// Set the graphic pipeline state.
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = vertexDeclartion.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);

		// Update shader uniform parameters.
		// VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), CompiledCameraModel, DisplacementMapResolution);
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(),
										TextureXAxisRHI,
										TextureYAxisRHI,
										TextureZAxisRHI/*,
										InMaxWindVelocity*/);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_WindExportVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_WindExportIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}

#pragma optimize("",off)
void UWindExportLibrary::ExportWindTextures(
		const UObject* WorldContextObject,
		struct FWindVelocityTextures* WindVelocityTextures,
		class UTextureRenderTarget2D* OutputRenderTarget/*,
		float InMaxWindVelocity*/)
{
	check(IsInGameThread());
	
	const UWorld* World = WorldContextObject->GetWorld();
	
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FTextureRenderTargetResource* TextureRenderTargetReource  = OutputRenderTarget->GameThread_GetRenderTargetResource();
	FShaderResourceViewRHIRef TextureXAxisRHI = WindVelocityTextures->GetCurXAxisSRV();
	FShaderResourceViewRHIRef TextureYAxisRHI = WindVelocityTextures->GetCurYAxisSRV();
	FShaderResourceViewRHIRef TextureZAxisRHI = WindVelocityTextures->GetCurZAxisSRV();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI,TextureRenderTargetReource/*,InMaxWindVelocity*/](FRHICommandListImmediate& RHICmdList)
		{
			ExportWindTextures_RenderThread(RHICmdList,FeatureLevel,TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI,TextureRenderTargetReource/*,InMaxWindVelocity*/);
		}
	);
}
#pragma optimize("",on)
