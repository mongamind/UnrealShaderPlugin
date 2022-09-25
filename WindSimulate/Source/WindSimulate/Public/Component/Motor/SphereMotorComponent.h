// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindMotorBaseComponent.h"
#include "Components/ActorComponent.h"
#include "Component/Motor/WindMotorBaseComponent.h"
#include "Motor/SphereMotorData.h"
#include "SphereMotorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API USphereMotorComponent : public UWindMotorBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USphereMotorComponent();
	
	virtual class FWindMotorBaseParamBase* GetWindMotorParam() override;

public:

	FSphereMotorParam SphereWindParam;
};
