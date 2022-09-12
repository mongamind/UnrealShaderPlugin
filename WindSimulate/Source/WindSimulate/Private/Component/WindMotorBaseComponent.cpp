// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WindMotorBaseComponent.h"

#include "Component/WindCenterComponent.h"
#include "GameFramework/Actor.h"
#include "Manager/WindSimulateManager.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UWindMotorBaseComponent::UWindMotorBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWindMotorBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UWindSimulateManager* WindSimulateManager = UWindSimulateManager::GetInstance(GetWorld());
	if(WindSimulateManager)
		WindSimulateManager->RegisterWindMotorComponent(this);
}

void UWindMotorBaseComponent::BeginDestroy()
{
	UWindSimulateManager* WindSimulateManager = UWindSimulateManager::GetInstance(GetWorld());
	if(WindSimulateManager)
		WindSimulateManager->UnregisterWindMotorComponent(this);
	
	Super::BeginDestroy();
}

bool UWindMotorBaseComponent::GetWindMotorParamInternal(FWindMotorBaseParamBase* OutMotorParam)
{
	if(OutMotorParam)
	{
		OutMotorParam->Radius = Radius;

		AActor* Actor = GetOwner();
		if(Actor)
		{
			OutMotorParam->MotorWorldSpacePos = Actor->GetActorLocation();
			OutMotorParam->WindVelocity = Actor->GetActorForwardVector() * WindForce;
			return true;
		}
	}

	return false;
}


