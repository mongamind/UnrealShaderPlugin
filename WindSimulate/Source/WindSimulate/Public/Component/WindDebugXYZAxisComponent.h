// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "WindDebugXYZAxisComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API UWindDebugXYZAxisComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWindDebugXYZAxisComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	bool bIsDebug = false;

	UPROPERTY(Transient)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void InitComponent();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> DisplayPlaneMesh;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UTextureRenderTarget2D> SampleRenderTarget;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Padding = 5;

	UPROPERTY(Transient)
	UTextureRenderTarget2D* SampleRenderTargetObj;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	FVector PlaneRelativePos = FVector(74,0,20);
	
};
