// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindMotorLibrary.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2


void UWindMotorLibrary::ApplyWindMotors_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FVector PlayerWorldPos,
		float MaxVelocity,
		float TexelsPerMeter,
		const TArray<FWindMotorBaseParamBase*>& AllWindMotors,
		FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
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

	// WindVelocityTexturesDoubleBuffer->SwapXAxisBuff();
	// WindVelocityTexturesDoubleBuffer->SwapYAxisBuff();
	// WindVelocityTexturesDoubleBuffer->SwapZAxisBuff();
	// const FWindVelocityTextures& DiffusionTextures = WindVelocityTexturesDoubleBuffer->GetCurVelocityTextures();

	
	// DiffusionTextures.ClearUAVToBlack(RHICmdList);

	
	DirectionalWindComputeShader->SetParameters(RHICmdList,PlayerWorldPos,MaxVelocity,TexelsPerMeter,AllDirectionalMotors,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurXAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisSRV());

	

	DispatchComputeShader(RHICmdList,DirectionalWindComputeShader,1,1,1);

	
	FComputeFenceRHIRef DirectionalMotorFence = RHICmdList.CreateComputeFence(TEXT("DirectionalMotor"));
	DirectionalWindComputeShader->UnsetParameters(RHICmdList,
												EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												DirectionalMotorFence);

	WindVelocityTexturesDoubleBuffer->SwapXAxisBuff();
	WindVelocityTexturesDoubleBuffer->SwapYAxisBuff();
	WindVelocityTexturesDoubleBuffer->SwapZAxisBuff();
}


void UWindMotorLibrary::ApplyWindMotors(
	const UObject* WorldContextObject,
	FWindMotorParamData &WindMotordData)
{
	check(IsInGameThread());
	
	const UWorld* World = WorldContextObject->GetWorld();
	
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	FVector PlayerWorldPos = WindMotordData.PlayerWorldPos;
	float MaxVelocity = WindMotordData.MaxVelocity;
	float TexelsPerMeter = WindMotordData.TexelsPerMeter;;
	TArray<FWindMotorBaseParamBase*> AllWindMotors = WindMotordData.AllWindMotors;
	FWindVelocityTextures* WindVelocityTexturesDoubleBuffer = WindMotordData.WindVelocityTexturesDoubleBuffer;
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,PlayerWorldPos,MaxVelocity,TexelsPerMeter,AllWindMotors,WindVelocityTexturesDoubleBuffer](FRHICommandListImmediate& RHICmdList)
		{
			ApplyWindMotors_RenderThread(RHICmdList,FeatureLevel,PlayerWorldPos ,MaxVelocity, TexelsPerMeter, AllWindMotors ,WindVelocityTexturesDoubleBuffer);
		}
	);
	
	
}
