// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindMotorLibrary.h"
#include "DirectionalMotorData.h"
#include "SphereMotorData.h"
#include "VortexMotorData.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#define DIFFUSIONTIMES_EVERFRAME 2


void UWindMotorLibrary::ApplyWindMotors_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FVector PlayerWorldPos,
		float MaxVelocity,
		FVector MetersPerTexel,
		const TArray<FWindMotorBaseParamBase*>& AllWindMotors,
		FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
{
	check(IsInRenderingThread())
	

	TArray<FDirectionalMotorParam*> AllDirectionalMotors;
	TArray<FSphereMotorParam*> AllSphereMotors;
	TArray<FVortexMotorParam*> AllVortexMotors;
	for(auto MotorParam : AllWindMotors)
	{
		if(MotorParam->GetWindMotorType() == EWindMotorType::WMT_Directional)
		{
			AllDirectionalMotors.Add((FDirectionalMotorParam*)(MotorParam)); 
		}
		else if(MotorParam->GetWindMotorType() == EWindMotorType::WMT_Sphere)
		{
			AllSphereMotors.Add((FSphereMotorParam*)(MotorParam)); 
		}
		else if(MotorParam->GetWindMotorType() == EWindMotorType::WMT_Vortex)
		{
			AllVortexMotors.Add((FVortexMotorParam*)(MotorParam)); 
		}
	}

	WindVelocityTexturesDoubleBuffer->ClearCurXAxisUAVToBlack(RHICmdList);
	WindVelocityTexturesDoubleBuffer->ClearCurYAxisUAVToBlack(RHICmdList);
	WindVelocityTexturesDoubleBuffer->ClearCurZAxisUAVToBlack(RHICmdList);

	if(AllDirectionalMotors.Num() > 0)
	{
		ApplyDirectionWindMotors_RenderThread(RHICmdList,FeatureLevel,PlayerWorldPos,MaxVelocity,
												MetersPerTexel,AllDirectionalMotors,WindVelocityTexturesDoubleBuffer);
	
		WindVelocityTexturesDoubleBuffer->SwapAllBuffer();
	}
	
	if(AllSphereMotors.Num() > 0)
	{
		ApplySphereWindMotors_RenderThread(RHICmdList,FeatureLevel,PlayerWorldPos,MaxVelocity,
												MetersPerTexel,AllSphereMotors,WindVelocityTexturesDoubleBuffer);
	
		WindVelocityTexturesDoubleBuffer->SwapAllBuffer();
	}
		
	
	if(AllVortexMotors.Num() > 0)
	{
		ApplyVortexWindMotors_RenderThread(RHICmdList,FeatureLevel,PlayerWorldPos,MaxVelocity,
												MetersPerTexel,AllVortexMotors,WindVelocityTexturesDoubleBuffer);

		WindVelocityTexturesDoubleBuffer->SwapAllBuffer();
	}

}

void UWindMotorLibrary::ApplyDirectionWindMotors_RenderThread(
				FRHICommandListImmediate& RHICmdList,
				ERHIFeatureLevel::Type FeatureLevel,
				FVector PlayerWorldPos,
				float MaxVelocity,
				FVector MetersPerTexel,
				const TArray<FDirectionalMotorParam*>& AllWindMotors,
				FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
{

	TShaderMapRef<FDirectionalMotorShader> DirectionalWindComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(DirectionalWindComputeShader.GetComputeShader());
	DirectionalWindComputeShader->SetParameters(RHICmdList,PlayerWorldPos,MaxVelocity,MetersPerTexel,AllWindMotors,
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
}

void UWindMotorLibrary::ApplySphereWindMotors_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FVector PlayerWorldPos,
		float MaxVelocity,
		FVector MetersPerTexel,
		const TArray<FSphereMotorParam*>& AllWindMotors,
		FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
{
	TShaderMapRef<FSphereMotorShader> SphereWindComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(SphereWindComputeShader.GetComputeShader());
	SphereWindComputeShader->SetParameters(RHICmdList,PlayerWorldPos,MaxVelocity,MetersPerTexel,AllWindMotors,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurXAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisSRV());

	

	DispatchComputeShader(RHICmdList,SphereWindComputeShader,1,1,1);

	
	FComputeFenceRHIRef SphereMotorFence = RHICmdList.CreateComputeFence(TEXT("SphereMotor"));
	SphereWindComputeShader->UnsetParameters(RHICmdList,
												EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												SphereMotorFence);
}

void UWindMotorLibrary::ApplyVortexWindMotors_RenderThread(
		FRHICommandListImmediate& RHICmdList,
		ERHIFeatureLevel::Type FeatureLevel,
		FVector PlayerWorldPos,
		float MaxVelocity,
		FVector MetersPerTexel,
		const TArray<FVortexMotorParam*>& AllWindMotors,
		FWindVelocityTextures* WindVelocityTexturesDoubleBuffer)
{
	TShaderMapRef<FVortexMotorShader> VortexWindComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(VortexWindComputeShader.GetComputeShader());
	VortexWindComputeShader->SetParameters(RHICmdList,PlayerWorldPos,MaxVelocity,MetersPerTexel,AllWindMotors,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurXAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisSRV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisSRV());

	

	DispatchComputeShader(RHICmdList,VortexWindComputeShader,1,1,1);

	
	FComputeFenceRHIRef VortexMotorFence = RHICmdList.CreateComputeFence(TEXT("VortexMotor"));
	VortexWindComputeShader->UnsetParameters(RHICmdList,
												EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EComputeToCompute,
												WindVelocityTexturesDoubleBuffer->GetCurXAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurYAxisUAV(),
												WindVelocityTexturesDoubleBuffer->GetCurZAxisUAV(),
												VortexMotorFence);
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
	FVector MetersPerTexel = WindMotordData.MetersPerTexel;;
	TArray<FWindMotorBaseParamBase*> AllWindMotors = WindMotordData.AllWindMotors;
	FWindVelocityTextures* WindVelocityTexturesDoubleBuffer = WindMotordData.WindVelocityTexturesDoubleBuffer;
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[FeatureLevel,PlayerWorldPos,MaxVelocity,MetersPerTexel,AllWindMotors,WindVelocityTexturesDoubleBuffer](FRHICommandListImmediate& RHICmdList)
		{
			ApplyWindMotors_RenderThread(RHICmdList,FeatureLevel,PlayerWorldPos ,MaxVelocity, MetersPerTexel, AllWindMotors ,WindVelocityTexturesDoubleBuffer);
		}
	);
	
	
}
