// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindMotorBaseComponent.h"
#include "Components/ActorComponent.h"
#include "Component/Motor/WindMotorBaseComponent.h"
#include "Motor/DirectionalMotorData.h"
#include "DirectionalMotorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API UDirectionalMotorComponent : public UWindMotorBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDirectionalMotorComponent();
	
	virtual class FWindMotorBaseParamBase* GetWindMotorParam() override;

public:

	FDirectionalMotorParam DirctionalWindParam;

	UPROPERTY(EditAnywhere,Category="Wind Simulate",DisplayName="Distance (Meter)")
	float Distance = 10;
};
