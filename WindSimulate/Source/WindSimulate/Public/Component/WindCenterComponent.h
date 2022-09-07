// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Diffusion/WindDiffusionData.h"
#include "Motor/WindMotorData.h"
#include "WindCenterComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API UWindCenterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWindCenterComponent();

	static class UWindCenterComponent* s_Instance;
	static class UWindCenterComponent* GetInstance();

	void GetWindSettingParam(FWindSetting& WindSetting);

	float GeMaxWindVelocity(){return MaxWindVelocity;}
	FVector GetMetersPerTexel(){return MetersPerTexel;}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	FVector MetersPerTexel = FVector(1,2,1);             
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	float MaxWindVelocity = 100;                   //Max of wind velocity.
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	float OriginalAlpha = 0.9f;
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	float BetaNearAdd = 0.14f;
	
	bool bIsFirstRecord = true;
	FVector PosLastRecord;
	
public:
	UPROPERTY(EditAnywhere,Category="Wind Simulate")
	UTextureRenderTarget2D* ExportLUT;
};
