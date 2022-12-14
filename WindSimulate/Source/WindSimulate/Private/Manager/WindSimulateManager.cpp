// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/WindSimulateManager.h"
#include "Component/WindCenterComponent.h"
#include "Component/Motor/WindMotorBaseComponent.h"
#include "Diffusion/WindDiffusionLibrary.h"
#include "Motor/WindMotorLibrary.h"
#include "Engine/World.h"
#include "Export/WindExportLibrary.h"


UWindSimulateManager* UWindSimulateManager::s_Instance = nullptr;

UWindSimulateManager* UWindSimulateManager::GetInstance(UObject* WorldContent)
{
	return s_Instance;
}

bool UWindSimulateManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

/** Implement this for initialization of instances of the system */
void UWindSimulateManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	s_Instance = this;
	InitWindVelocityTextureBuffers();
}

/** Implement this for deinitialization of instances of the system */
void UWindSimulateManager::Deinitialize()
{
	s_Instance = nullptr;
	ReleaseWindVelocityTextureBuffers();
	Super::Deinitialize();
}


void UWindSimulateManager::RegisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent)
{
	WindMotorsRegistered.Add(MotorComponent);
}

void UWindSimulateManager::UnregisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent)
{
	WindMotorsRegistered.Remove(MotorComponent);
}

void UWindSimulateManager::SetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent)
{
	PlayerHoldComponent = InPlayerHoldComponent;
}

void UWindSimulateManager::UnsetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent)
{
	if(PlayerHoldComponent == InPlayerHoldComponent)
		PlayerHoldComponent = nullptr;
}

void UWindSimulateManager::RegisterWindDebugArrowsComponent(class UWindDebugArrowsComponent* MotorComponent)
{
	WindDebugArrowsRegistered = MotorComponent;
}

void UWindSimulateManager::UnregisterWindDebugArrowsComponent(class UWindDebugArrowsComponent* MotorComponent)
{
	WindDebugArrowsRegistered = MotorComponent;
}

float UWindSimulateManager::GetMaxWindVelocity()
{
	if(PlayerHoldComponent)
		return PlayerHoldComponent->GeMaxWindVelocity();
	
	return 1.0f;
}

FVector UWindSimulateManager::GetWindCenterWorldPos()
{
	if(PlayerHoldComponent)
	{
		AActor* PlayerActor = PlayerHoldComponent->GetOwner();
		if (PlayerActor)
			return PlayerActor->GetActorLocation();
	}
	
	return FVector::ZeroVector;
}

void UWindSimulateManager::Tick( float DeltaTime )
{
	if(PlayerHoldComponent == nullptr)
		return;
	
	AActor* PlayerActor = PlayerHoldComponent->GetOwner();
	if(!PlayerActor)
		return;

	// if(RunCount > 1)
	// 	return;

	++RunCount;
		
#pragma region Diffusion	
	//Diffusion
	{
		PlayerHoldComponent->GetWindSettingParam(WindFieldData.WindSetting);
		WindFieldData.WindVelocityTexturesDoubleBuffer = &WindVelocityTextureBuffer;
	
		UWindDiffusionLibrary::DrawWindDiffusion(GetWorld(),WindFieldData);		
	}
#pragma endregion Diffusion

	
#pragma region Apply Motors
	WindMotorData.MaxVelocity = PlayerHoldComponent->GeMaxWindVelocity();
	WindMotorData.WindCenterWorldPos = PlayerActor->GetActorLocation();
	WindMotorData.MetersPerTexel = PlayerHoldComponent->GetMetersPerTexel();
	
	TArray<FWindMotorBaseParamBase*>& MotorParams = WindMotorData.AllWindMotors;
	MotorParams.Empty();
	for(UWindMotorBaseComponent* MotorComponent : WindMotorsRegistered)
	{
		if(MotorComponent && !MotorComponent->IsPendingKill())
		{
			FWindMotorBaseParamBase* MotorParam = MotorComponent->GetWindMotorParam();
			if(MotorParam)
				MotorParams.Add(MotorParam);
		}
	}
	
	if(MotorParams.Num() > 0)
	{
		WindMotorData.WindVelocityTexturesDoubleBuffer = &WindVelocityTextureBuffer;
		UWindMotorLibrary::ApplyWindMotors(GetWorld(),WindMotorData);
	}
#pragma endregion Apply Motors

#pragma region readback to cpu
	if (PlayerHoldComponent->ExportLUT)
		UWindExportLibrary::ExportWindTextures(GetWorld(),&WindVelocityTextureBuffer,PlayerHoldComponent->ExportLUT/*,PlayerHoldComponent->GeMaxWindVelocity()*/);
#pragma endregion 
}

TStatId UWindSimulateManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UWindSimulateManager, STATGROUP_Tickables);
}

void UWindSimulateManager::InitWindVelocityTextureBuffers()
{
	WindVelocityTextureBuffer.Init(EPixelFormat::PF_R32_FLOAT,32,32,16);
}

void UWindSimulateManager::ReleaseWindVelocityTextureBuffers()
{
	WindVelocityTextureBuffer.Release();
}

bool UWindSimulateManager::RefreshExportData()
{
	if(PlayerHoldComponent && PlayerHoldComponent->ExportLUT)
	{
		return ExportData.ReadPixelsFromRenderTarget2D(PlayerHoldComponent->ExportLUT);	
	}


	return false;
}
