﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SphereMotorComponent.h"


// Sets default values for this component's properties
USphereMotorComponent::USphereMotorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

class FWindMotorBaseParamBase* USphereMotorComponent::GetWindMotorParam()
{
	GetWindMotorParamInternal(&SphereWindParam);

	return &SphereWindParam;
}

