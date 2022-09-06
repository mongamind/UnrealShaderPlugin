// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WindDebugArrowsComponent.h"
#include "Component/WindSimulateManager.h"

#define XAsixCount int(32 * 0.5)
#define YAsixCount int(16 * 0.5)
#define ZAsixCount int(32 * 0.5)

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

	ConstructArrows();
}

void UWindDebugArrowsComponent::BeginDestroy()
{
	Super::BeginDestroy();

	UWindSimulateManager* SimulateMgr = UWindSimulateManager::GetInstance(this);
	if(SimulateMgr)
		SimulateMgr->UnregisterWindDebugArrowsComponent(this);
}

void UWindDebugArrowsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AActor* Actor = GetOwner();
	FVector ActorLocation = Actor->GetActorLocation();
	ActorLocation = FVector(int(ActorLocation.X / 100) * 100, int(ActorLocation.Y / 100) * 100, int(ActorLocation.Z / 100) * 100);
	for(int X = 0;X < XAsixCount;X++)
	{
		for(int Y = 0;Y < YAsixCount;Y++)
		{
			for(int Z = 0;Z < ZAsixCount;Z++)
			{
				UStaticMeshComponent* ArrowComp = GetArrowWithGridPos(X,Y,Z);
				if(ArrowComp)
				{
					ArrowComp->SetWorldLocation(ActorLocation + FVector((X - XAsixCount * 0.5) * 100,(Y - YAsixCount * 0.5) * 100,(Z - ZAsixCount * 0.5) * 100));
					ArrowComp->SetWorldRotation(FRotator::ZeroRotator);
				}
			}
		}
	}
}

void UWindDebugArrowsComponent::ConstructArrows()
{
	AActor* Actor = GetOwner();
	FVector ActorLocation = Actor->GetActorLocation();
	ActorLocation = FVector(int(ActorLocation.X / 100) * 100, int(ActorLocation.Y / 100) * 100, int(ActorLocation.Z / 100) * 100);
	for(int X = 0;X < XAsixCount;X++)
	{
		for(int Y = 0;Y < YAsixCount;Y++)
		{
			for(int Z = 0;Z < ZAsixCount;Z++)
			{
				UStaticMeshComponent* Arrow = NewObject<UStaticMeshComponent>(Actor,UStaticMeshComponent::StaticClass());
				Actor->AddInstanceComponent(Arrow);
				Arrow->AttachToComponent(Actor->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
				Arrow->RegisterComponent();
				Arrow->SetWorldLocation(ActorLocation + FVector((X - XAsixCount * 0.5) * 100,(Y - YAsixCount * 0.5) * 100,(Z - ZAsixCount * 0.5) * 100));
				Arrow->SetWorldRotation(FRotator::ZeroRotator);
				Arrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				Arrow->SetStaticMesh(ArrowMesh);

				
				
				AllArrows.Add(Arrow);
			}
		}
	}
}

UStaticMeshComponent* UWindDebugArrowsComponent::GetArrowWithGridPos(int InX,int InY,int InZ)
{
	int Index = InX * (YAsixCount * ZAsixCount) + InY * ZAsixCount + InZ;
	if(AllArrows.IsValidIndex(Index))
		return AllArrows[Index];

	return nullptr;
}