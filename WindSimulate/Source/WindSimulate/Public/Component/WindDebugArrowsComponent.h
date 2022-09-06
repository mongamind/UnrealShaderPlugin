// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WindDebugArrowsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WINDSIMULATE_API UWindDebugArrowsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWindDebugArrowsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ConstructArrows();
	UStaticMeshComponent* GetArrowWithGridPos(int InX,int InY,int InZ);

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UStaticMesh* ArrowMesh;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UMaterial* ArrowMaterial;


	UPROPERTY(BlueprintReadWrite,Transient)
	TArray<UStaticMeshComponent*> AllArrows;
};
