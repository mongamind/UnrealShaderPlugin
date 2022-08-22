// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindDebugLibrary.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2

TGlobalResource<FWindDebugVertexBuffer> g_WindDebugVertexBuffer;
TGlobalResource<FWindDebugIndexBuffer> g_WindDebugIndexBuffer;

void UWindDebugLibrary::DrawWindTextureToRT_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FTextureRenderTargetResource* OutTextureRenderTargetResource,
		float Padding,
		float MaxWindVelocity,
		FShaderResourceViewRHIRef TextureXAxisRHI,
		FShaderResourceViewRHIRef TextureYAxisRHI,
		FShaderResourceViewRHIRef TextureZAxisRHI)
{
	check(IsInRenderingThread());

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();
	RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, RenderTargetTexture);

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store, OutTextureRenderTargetResource->TextureRHI);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("WindDebugShaderPass"));
	{
		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef< FWindDebugShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FWindDebugShaderPS > PixelShader(GlobalShaderMap);

		FWindDebugVertexDeclaration vertexDeclartion;
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
		VertexShader->SetParameters(RHICmdList,VertexShader.GetVertexShader(), Padding,MaxWindVelocity, TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI);
		PixelShader->SetParameters(RHICmdList,PixelShader.GetPixelShader(), Padding,MaxWindVelocity, TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_WindDebugVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_WindDebugIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}


void UWindDebugLibrary::DrawWindTextureToRT(
	const UObject* WorldContextObject,
	float Padding,
	float MaxWindVelocity,
	FWindVelocityTextures* WindTextureBuffers,
	class UTextureRenderTarget2D* OutputRenderTarget)
{
	
	check(IsInGameThread());

	if(!OutputRenderTarget)
		return;

	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();
	FTextureRenderTargetResource* TextureRenderTargetReource  = OutputRenderTarget->GameThread_GetRenderTargetResource();
	
	FShaderResourceViewRHIRef TextureXAxisRHI = WindTextureBuffers->GetCurXAxisSRV();
	FShaderResourceViewRHIRef TextureYAxisRHI = WindTextureBuffers->GetCurYAxisSRV();
	FShaderResourceViewRHIRef TextureZAxisRHI = WindTextureBuffers->GetCurZAxisSRV();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetReource,FeatureLevel,Padding,MaxWindVelocity,TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI](FRHICommandListImmediate& RHICmdList)
		{
			DrawWindTextureToRT_RenderThread(RHICmdList,FeatureLevel,TextureRenderTargetReource,Padding,MaxWindVelocity,TextureXAxisRHI,TextureYAxisRHI,TextureZAxisRHI);
		}
	);
}
