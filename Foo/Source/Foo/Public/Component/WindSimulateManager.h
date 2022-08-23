// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "Diffusion/WindDiffusionData.h"
#include "Motor/WindMotorData.h"
#include "WindSimulateManager.generated.h"

/**
 * 
 */
UCLASS()
class UWindSimulateManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	static UWindSimulateManager* s_Instance;
public:
	static UWindSimulateManager* GetInstance(UObject* WorldContent);
	
	bool ShouldCreateSubsystem(UObject* Outer);
	void Initialize(FSubsystemCollectionBase& Collection);
	void Deinitialize();
	
	void RegisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent);
	void UnregisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent);

	void SetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent);
	void UnsetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent);

	float GetMaxWindVelocity();

	FWindVelocityTextures& GetWindVelocityBuffer(){return WindVelocityTextureBuffer;};

protected:
	virtual void Tick( float DeltaTime ) override;
	virtual TStatId GetStatId() const override;

	void InitWindVelocityTextureBuffers();

	void ReleaseWindVelocityTextureBuffers();

	friend class UWindSimulateDebugComponent;

private:
	UPROPERTY(Transient)
	TSet<class UWindMotorBaseComponent*> WindMotorsRegistered;

	UPROPERTY(Transient)
	class UWindCenterComponent* PlayerHoldComponent;

	FWindVelocityFieldDataParam WindFieldData;
	FWindMotorParamData WindMotorData;

	FWindVelocityTextures WindVelocityTextureBuffer;


	int RunCount = 0;
};
