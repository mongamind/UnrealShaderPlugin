// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Motor/DirectionalMotorComponent.h"


// Sets default values for this component's properties
UDirectionalMotorComponent::UDirectionalMotorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

PRAGMA_DISABLE_OPTIMIZATION
class FWindMotorBaseParamBase* UDirectionalMotorComponent::GetWindMotorParam()
{
	GetWindMotorParamInternal(&DirctionalWindParam);

	FVector NormalizeDir = GetOwner()->GetActorForwardVector();
	NormalizeDir.Normalize();
	DirctionalWindParam.Direction = NormalizeDir;
	DirctionalWindParam.Distance = Distance;

	return &DirctionalWindParam;
}
PRAGMA_ENABLE_OPTIMIZATION

