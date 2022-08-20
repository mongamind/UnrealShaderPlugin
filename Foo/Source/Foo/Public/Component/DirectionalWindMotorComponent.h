// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindMotorBaseComponent.h"
#include "Components/ActorComponent.h"
#include "Component/WindMotorBaseComponent.h"
#include "Motor/WindMotorData.h"
#include "DirectionalWindMotorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FOO_API UDirectionalWindMotorComponent : public UWindMotorBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDirectionalWindMotorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:

	virtual class FWindMotorBaseParamBase* GetWindMotorParam() override;

public:

	FDirectionalWindMotorParam DirctionalWindParam;
};
