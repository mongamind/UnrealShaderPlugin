// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/Object.h"
#include "Tickable.h"
#include "Diffusion/WindDiffusionData.h"
#include "Motor/WindMotorData.h"
#include "Utility/Float16ColorArrayWrapper.h"
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
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void RegisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent);
	void UnregisterWindMotorComponent(class UWindMotorBaseComponent* MotorComponent);

	void SetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent);
	void UnsetPlayerHoldComponent(class UWindCenterComponent* InPlayerHoldComponent);

	void RegisterWindDebugArrowsComponent(class UWindDebugArrowsComponent* MotorComponent);
	void UnregisterWindDebugArrowsComponent(class UWindDebugArrowsComponent* MotorComponent);
	UWindDebugArrowsComponent* GetWindDebugArrowsComponent(){return WindDebugArrowsRegistered;}

	float GetMaxWindVelocity();
	FVector GetWindCenterWorldPos();

	FWindVelocityTextures& GetWindVelocityBuffer(){return WindVelocityTextureBuffer;};

	bool RefreshExportData();
	const FFloat16ColorArrayWrapper& GetExportData(){return ExportData;};

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

	UPROPERTY(Transient)
	class UWindDebugArrowsComponent* WindDebugArrowsRegistered;

	FFloat16ColorArrayWrapper ExportData;


	FWindVelocityFieldDataParam WindFieldData;
	FWindMotorParamData WindMotorData;

	FWindVelocityTextures WindVelocityTextureBuffer;

	TSharedPtr<struct FWindPingPongPBO> ReadBackPBO; 


	int RunCount = 0;
};
