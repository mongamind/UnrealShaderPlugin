// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WindMotorBaseComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API UWindMotorBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWindMotorBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:

	virtual class FWindMotorBaseParamBase* GetWindMotorParam() {return nullptr;};


	UPROPERTY(EditAnywhere,Category="Wind Simulate",DisplayName="Radius (Meter)")
	float Radius = 5;

	
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	float WindForce = 100;

	
protected:
	virtual bool GetWindMotorParamInternal(FWindMotorBaseParamBase* OutMotorParam);
	
	bool bHasRegister = false;
	
	
};
