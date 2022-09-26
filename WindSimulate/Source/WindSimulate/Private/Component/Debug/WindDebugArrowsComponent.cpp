// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Debug/WindDebugArrowsComponent.h"

#include "Kismet/KismetArrayLibrary.h"
#include "Manager/WindSimulateManager.h"
#include "Kismet/KismetSystemLibrary.h"

#define XAsixCount 32
#define YAsixCount 16
#define ZAsixCount 32

#define LUT_COLUMN_COUNT 4

#define LUTTexture_Width (XAsixCount * LUT_COLUMN_COUNT)
#define LUTTexture_Height (ZAsixCount * LUT_COLUMN_COUNT)


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

		FVector DebugWorldPos = OwnerActor->GetActorLocation();

		float MaxWindVelocity = SimulateMgr->GetMaxWindVelocity();
		FVector WindCenterWorldPos = SimulateMgr->GetWindCenterWorldPos();
		
		FIntVector DebugGridPosDelta = FIntVector(FVector(DebugWorldPos - WindCenterWorldPos) / 100);

		// FVector2D RealPixelRate(XAsixCount / (XAsixCount + LUT_PADDING),ZAsixCount / (ZAsixCount + LUT_PADDING));
		
		const FFloat16ColorArrayWrapper& ExportData = SimulateMgr->GetExportData();
		if(ExportData.bIsSuccessed)
		{
			for(int X = 0;X < XAsixCount;X++)
			{
				for(int Y = 0;Y < YAsixCount;Y++)
				{
					for(int Z = 0;Z < ZAsixCount;Z++)
			// for(int X = int(XAsixCount / 2)  ;X <= int(XAsixCount / 2) ;X++)
			// {
			// 	for(int Y = int(YAsixCount / 2)  ;Y <= int(YAsixCount / 2) ;Y++)
			// 	{
			// 		for(int Z = int(ZAsixCount / 2) ;Z <= int(ZAsixCount / 2) ;Z++)
			// for(int X = int(XAsixCount / 2)  ;X <= int(XAsixCount / 2) ;X++)
			// {
			// 	for(int Y = int(YAsixCount / 2)  ;Y <= int(YAsixCount / 2) ;Y++)
			// 	{
			// 		for(int Z = int(ZAsixCount /2)  ;Z <= int(ZAsixCount / 2) ;Z++)
					{
						FIntVector GridPos = FIntVector(X,Y,Z) + DebugGridPosDelta;
						
						int LayerIndexX = GridPos.Y % LUT_COLUMN_COUNT;
						int LayerIndexY = GridPos.Y / LUT_COLUMN_COUNT;
						int xIndex = GridPos.X + LayerIndexX * ZAsixCount ;
						int yIndex = (ZAsixCount - GridPos.Z - 1) + LayerIndexY * ZAsixCount ;
						
						int DataIndex = ExportData.GetIndex(xIndex,yIndex);

						FVector WindVelocity = ExportData.Color(DataIndex);

						// UE_LOG(LogTemp,Log,TEXT("X:%d Y:%d Z:%d LayerIndex.X:%d LayerIndex.Y:%d DataIndex:%d WindVelocity:%f,%f,%f "),X,Y,Z,LayerIndexX,LayerIndexY,DataIndex,WindVelocity.X,WindVelocity.Y,WindVelocity.Z);

						FVector PosStart =  DebugWorldPos - FVector((X - XAsixCount * 0.5f + 1) * 100,(Y - YAsixCount * 0.5f + 1) * 100,(Z - ZAsixCount * 0.5f + 1) * 100);

						FVector VectorNormalize = WindVelocity.GetSafeNormal();
						FVector PosEnd = PosStart + (VectorNormalize + WindVelocity) * VolecityDisplayRate;

						float LenSquared = WindVelocity.SizeSquared();
						if(LenSquared > 1)
						{
							FLinearColor ArrowColorTemp(LenSquared * 0.001f,0,0,1);
							UKismetSystemLibrary::DrawDebugLine(GetWorld(),PosStart,PosEnd,ArrowColorTemp);
							UKismetSystemLibrary::DrawDebugArrow(GetWorld(),PosStart,PosEnd,10,ArrowColorTemp);
						}
					}
				}
			}
		}
	}

}

FVector2D UWindDebugArrowsComponent::GetDebugCenterUVInRT()
{
	AActor* OwnerActor = GetOwner();
	UWindSimulateManager* SimulateMgr = UWindSimulateManager::GetInstance(this);
	if(SimulateMgr && OwnerActor)
	{
		FVector DebugWorldPos = OwnerActor->GetActorLocation();
		FVector WindCenterWorldPos = SimulateMgr->GetWindCenterWorldPos();
		FIntVector DebugGridPosDelta = FIntVector(FVector(DebugWorldPos - WindCenterWorldPos) / 100);

		FIntVector GridPos = FIntVector(XAsixCount / 2,YAsixCount / 2,ZAsixCount / 2) + DebugGridPosDelta;

		int LayerIndexX = GridPos.Y % LUT_COLUMN_COUNT;
		int LayerIndexY = GridPos.Y / LUT_COLUMN_COUNT;
		int xIndex = GridPos.X + LayerIndexX * ZAsixCount ;
		int yIndex = (ZAsixCount - GridPos.Z - 1) + LayerIndexY * ZAsixCount ;
		
		return FVector2D(xIndex / (float)LUTTexture_Width,yIndex / (float)LUTTexture_Height);
	}
	
	return FVector2D(0.5,0.5);
}

PRAGMA_ENABLE_OPTIMIZATION