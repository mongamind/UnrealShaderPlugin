// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Debug/WindDebugRTDisplayComponent.h"

#include "Component/Debug/WindDebugArrowsComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Manager/WindSimulateManager.h"


// Sets default values for this component's properties
UWindDebugRTDisplayComponent::UWindDebugRTDisplayComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWindDebugRTDisplayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if(DisplayMeterial)
	{
		UMaterialInstanceDynamic* NewMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DisplayMeterial, GetTransientPackage());
		SetMaterial(0,NewMaterialInstanceDynamic);
	}
	
}


// Called every frame
void UWindDebugRTDisplayComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	UWindSimulateManager* WindSimulateMgr = UWindSimulateManager::GetInstance(GetWorld());
	if(WindSimulateMgr)
	{
		UWindDebugArrowsComponent* DebugArrowsComponent = WindSimulateMgr->GetWindDebugArrowsComponent();
		if(DebugArrowsComponent)
		{
			FVector2D DebugCenter = DebugArrowsComponent->GetDebugCenterUVInRT();

			UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(GetMaterial(0));
			if(MaterialInstanceDynamic)
			{
				MaterialInstanceDynamic->SetScalarParameterValue("DebugArrowCenter_X",DebugCenter.X);
				MaterialInstanceDynamic->SetScalarParameterValue("DebugArrowCenter_Y",DebugCenter.Y);
			}
		}
	}
}

