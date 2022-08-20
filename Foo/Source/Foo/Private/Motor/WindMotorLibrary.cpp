// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindMotorLibrary.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2


void UWindMotorLibrary::ApplyWindMotors_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		const TArray<FWindMotorBaseParamBase*>& AllWindMotors,
		FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer)
{
	check(IsInRenderingThread())

	TShaderMapRef<FWindDirectionalMotorShader> DirectionalWindComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(DirectionalWindComputeShader.GetComputeShader());

	TArray<FDirectionalWindMotorParam> AllDirectionalMotors;
	for(auto MotorParam : AllWindMotors)
	{
		if(MotorParam->GetWindMotorType() == EWindMotorType::WMT_Directional)
		{
			AllDirectionalMotors.Add(*(FDirectionalWindMotorParam*)(MotorParam)); 
		}
	}

		WindVelocityTexturesDoubleBuffer->SwapBuffer();
		const FWindVelocityTextures& DiffusionTextures = WindVelocityTexturesDoubleBuffer->GetCurVelocityTextures();

		FComputeFenceRHIRef DirectionalMotorFence = RHICmdList.CreateComputeFence(TEXT("DirectionalMotor"));
		DiffusionTextures.ClearUAVToBlack(RHICmdList);

		
		DirectionalWindComputeShader->SetParameters(RHICmdList,AllDirectionalMotors,
													DiffusionTextures.WindDiffusionXAxisTexture_UAV,
													DiffusionTextures.WindDiffusionXAxisTexture_SRV,
													DiffusionTextures.WindDiffusionYAxisTexture_UAV,
													DiffusionTextures.WindDiffusionYAxisTexture_SRV,
													DiffusionTextures.WindDiffusionZAxisTexture_UAV,
													DiffusionTextures.WindDiffusionZAxisTexture_SRV);
	
		
	
		DispatchComputeShader(RHICmdList,DirectionalWindComputeShader,1,1,1);
	
		DirectionalWindComputeShader->UnsetParameters(RHICmdList,
													EResourceTransitionAccess::EMetaData, EResourceTransitionPipeline::EComputeToCompute,
													DiffusionTextures.WindDiffusionXAxisTexture_UAV,
													DiffusionTextures.WindDiffusionYAxisTexture_UAV,
													DiffusionTextures.WindDiffusionZAxisTexture_UAV,
													DirectionalMotorFence);
}


void UWindMotorLibrary::ApplyWindMotors(
	const UObject* WorldContextObject,
	FWindMotorParamData &WindMotordData)
{
	check(IsInGameThread());
	
	const UWorld* World = WorldContextObject->GetWorld();
	
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	TArray<FWindMotorBaseParamBase*> AllWindMotors = WindMotordData.AllWindMotors;
	FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer = WindMotordData.WindVelocityTexturesDoubleBuffer;
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,AllWindMotors,WindVelocityTexturesDoubleBuffer](FRHICommandListImmediate& RHICmdList)
		{
			ApplyWindMotors_RenderThread(RHICmdList,FeatureLevel
											,AllWindMotors
											,WindVelocityTexturesDoubleBuffer);
		}
	);
	
	
}
