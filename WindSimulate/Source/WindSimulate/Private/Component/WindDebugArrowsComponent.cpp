// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WindDebugArrowsComponent.h"

#include "Kismet/KismetArrayLibrary.h"
#include "Manager/WindSimulateManager.h"
#include "Kismet/KismetSystemLibrary.h"

#define XAsixCount 32
#define YAsixCount 16
#define ZAsixCount 32

#define LUT_COLUMN_COUNT 4
#define LUT_PADDING 2
#define VolecityDisplayRate 1

// Sets default values for this component's properties
UWindDebugArrowsComponent::UWindDebugArrowsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWindDebugArrowsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UWindSimulateManager* SimulateMgr = UWindSimulateManager::GetInstance(this);
	if(SimulateMgr)
		SimulateMgr->RegisterWindDebugArrowsComponent(this);
}

void UWindDebugArrowsComponent::BeginDestroy()
{
	Super::BeginDestroy();

	UWindSimulateManager* SimulateMgr = UWindSimulateManager::GetInstance(this);
	if(SimulateMgr)
		SimulateMgr->UnregisterWindDebugArrowsComponent(this);
}

PRAGMA_DISABLE_OPTIMIZATION
void UWindDebugArrowsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AActor* OwnerActor = GetOwner();
	UWindSimulateManager* SimulateMgr = UWindSimulateManager::GetInstance(this);
	if(SimulateMgr && OwnerActor)
	{
		SimulateMgr->RefreshExportData();

		float MaxWindVelocity = SimulateMgr->GetMaxWindVelocity();

		FVector WorldPos = OwnerActor->GetActorLocation();

		// FVector2D RealPixelRate(XAsixCount / (XAsixCount + LUT_PADDING),ZAsixCount / (ZAsixCount + LUT_PADDING));
		
		const FFloat16ColorArrayWrapper& ExportData = SimulateMgr->GetExportData();
		if(ExportData.bIsSuccessed)
		{
			for(int X = 0;X < XAsixCount;X++)
			{
				for(int Y = 0;Y < YAsixCount;Y++)
				{
					for(int Z = 0;Z < ZAsixCount;Z++)
			
			// for(int X = XAsixCount * 0.5 - 1;X < XAsixCount * 0.5 + 1;X++)
			// {
			// 	for(int Y = YAsixCount * 0.5 - 1;Y < YAsixCount * 0.5 + 1;Y++)
			// 	{
			// 		for(int Z = ZAsixCount * 0.5;Z < ZAsixCount * 0.5 + 1;Z++)
					{
						int LayerIndexX = Y % LUT_COLUMN_COUNT;
						int LayerIndexY = Y / LUT_COLUMN_COUNT;
						int xIndex = X + LayerIndexX * XAsixCount;
						int yIndex = Z + LayerIndexY * ZAsixCount ;

						
						int DataIndex = ExportData.GetIndex(xIndex,yIndex);

						FVector WindVelocity = ExportData.Color(DataIndex);

						// UE_LOG(LogTemp,Log,TEXT("X:%d Y:%d Z:%d LayerIndex.X:%d LayerIndex.Y:%d DataIndex:%d WindVelocity:%f,%f,%f "),X,Y,Z,LayerIndexX,LayerIndexY,DataIndex,WindVelocity.X,WindVelocity.Y,WindVelocity.Z);

						FVector PosStart =  WorldPos + FVector((X - XAsixCount * 0.5f) * 100,(Y - YAsixCount * 0.5f) * 100,(Z - ZAsixCount * 0.5f) * 100);

						FVector VectorNormalize = WindVelocity.GetSafeNormal();
						FVector PosEnd = PosStart + (VectorNormalize + WindVelocity) * VolecityDisplayRate;
						
						
						UKismetSystemLibrary::DrawDebugLine(GetWorld(),PosStart,PosEnd,ArrowColor);
						UKismetSystemLibrary::DrawDebugArrow(GetWorld(),PosStart,PosEnd,10,ArrowColor);
					}
				}
			}
		}
	}
}

PRAGMA_ENABLE_OPTIMIZATION