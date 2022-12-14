// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Debug/WindDebugXYZAxisComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Logging/LogVerbosity.h"
#include "Logging/LogMacros.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/EngineTypes.h"
#include "Debug/WindDebugLibrary.h"
#include "Manager/WindSimulateManager.h"


// Sets default values for this component's properties
UWindDebugXYZAxisComponent::UWindDebugXYZAxisComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWindDebugXYZAxisComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitComponent();
}

void UWindDebugXYZAxisComponent::InitComponent()
{
	if(!StaticMeshComponent)
	{

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		AActor* Actor = GetOwner();
		if(Actor)
		{
			
			UCameraComponent* CameraComponent = Cast<UCameraComponent>(Actor->GetComponentByClass(UCameraComponent::StaticClass()));
			if(CameraComponent)
			{
				StaticMeshComponent = NewObject<UStaticMeshComponent>(GetOuter(),UStaticMeshComponent::StaticClass());
				Actor->AddInstanceComponent(StaticMeshComponent);
				StaticMeshComponent->AttachToComponent(CameraComponent,FAttachmentTransformRules::KeepRelativeTransform);
				StaticMeshComponent->RegisterComponent();

				StaticMeshComponent->SetRelativeLocation(PlaneRelativePos);
				StaticMeshComponent->SetRelativeRotation(FRotator(0,90,90));
				StaticMeshComponent->SetRelativeScale3D(FVector(1.2,0.0583,1));


				UStaticMeshComponent* StaticMeshComponentTemp = StaticMeshComponent;
				TSoftObjectPtr<UStaticMesh> DisplayPlaneMeshTemp = DisplayPlaneMesh;
				StreamableManager.RequestAsyncLoad(DisplayPlaneMesh.ToSoftObjectPath(),FStreamableDelegate::CreateLambda([this,StaticMeshComponentTemp,DisplayPlaneMeshTemp]()
				{
					StaticMeshComponentTemp->SetStaticMesh(DisplayPlaneMeshTemp.Get());
				}));
			}

			if(!SampleRenderTargetObj)
			{
				TSoftObjectPtr<UTextureRenderTarget2D> SampleRenderTargetTemp = SampleRenderTarget;
				StreamableManager.RequestAsyncLoad(SampleRenderTarget.ToSoftObjectPath(),FStreamableDelegate::CreateLambda([this,SampleRenderTargetTemp]()
				{
					SampleRenderTargetObj = SampleRenderTargetTemp.Get();
				}));
			}
		}
	}
}


// Called every frame
void UWindDebugXYZAxisComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if(SampleRenderTargetObj)
	{
		UWindSimulateManager* WindMgr = UWindSimulateManager::GetInstance(GetWorld());
		if(WindMgr)
		{
			// WindMgr->WindVelocityTextureBuffer.SwapBuffer();
			float MaxWindVelocity = WindMgr->GetMaxWindVelocity();
			UWindDebugLibrary::DrawWindXYZAxisToRT(GetWorld(),Padding,MaxWindVelocity,&WindMgr->GetWindVelocityBuffer(),SampleRenderTargetObj);
			// UColorRTShaderBlueprintLibrary::DrawSimpleColorToRenderTarget(GetWorld(),SampleRenderTargetObj,FLinearColor(1,0,0,1));
		}
		
		
	}
	
}

