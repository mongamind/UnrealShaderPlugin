// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WindCenterComponent.h"
#include "Component/Motor/WindMotorBaseComponent.h"
#include "Manager/WindSimulateManager.h"

#include "Diffusion/WindDiffusionLibrary.h"
#include "Motor/WindMotorLibrary.h"

#include "Engine/World.h"
#include "Math/Vector.h"


UWindCenterComponent* UWindCenterComponent::s_Instance = nullptr;
class UWindCenterComponent* UWindCenterComponent::GetInstance()
{
	return s_Instance;
}

// Sets default values for this component's properties
UWindCenterComponent::UWindCenterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWindCenterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//check(!s_Instance);
	s_Instance = this;

	UWindSimulateManager* WindSimulateManager = UWindSimulateManager::GetInstance(GetWorld());
	if(WindSimulateManager)
	{
		WindSimulateManager->SetPlayerHoldComponent(this);
	}
}

void UWindCenterComponent::BeginDestroy()
{
	//check(s_Instance == nullptr || s_Instance == this);
	s_Instance = nullptr;
	
	UWindSimulateManager* WindSimulateManager = UWindSimulateManager::GetInstance(GetWorld());
	if(WindSimulateManager)
	{
		WindSimulateManager->UnsetPlayerHoldComponent(this);
	}
	
	Super::BeginDestroy();
}

void UWindCenterComponent::GetWindSettingParam(FWindSetting& WindSetting)
{
	AActor* ActorOwner = GetOwner();
	if(ActorOwner)
	{
		if(bIsFirstRecord)
			WindSetting.InPosDelta = FVector::ZeroVector;
		else
			WindSetting.InPosDelta = ActorOwner->GetActorLocation() - PosLastRecord;

		PosLastRecord = ActorOwner->GetActorLocation();
		bIsFirstRecord = false;
	}
		
	
	WindSetting.MaxWindVelocity = MaxWindVelocity;
	WindSetting.OriginalAlpha = OriginalAlpha;
	WindSetting.BetaNearAdd = BetaNearAdd;
	WindSetting.MetersPerTexel = GetMetersPerTexel();
}

