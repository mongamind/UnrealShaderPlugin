// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Diffusion/WindDiffusionData.h"
#include "Motor/WindMotorData.h"
#include "WindCenterHoldComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FOO_API UWindCenterHoldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWindCenterHoldComponent();

	static class UWindCenterHoldComponent* s_Instance;
	static class UWindCenterHoldComponent* GetInstance();

	void GetWindSettingParam(FWindSetting& WindSetting); 

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:

	UPROPERTY(EditDefaultsOnly,Category="Wind Simulate")
	float InMaxWindVelocity = 100;                   //Max of wind velocity.
	UPROPERTY(EditDefaultsOnly,Category="Wind Simulate")
	float OriginalAlpha = 0.9f;
	UPROPERTY(EditDefaultsOnly,Category="Wind Simulate")
	float BetaNearAdd = 0.14f;

	bool bIsFirstRecord = true;
	FVector PosLastRecord;
};
