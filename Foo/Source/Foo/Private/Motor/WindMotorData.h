// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Diffusion/WindDiffusionData.h"

enum EWindMotorType
{
	WMT_Unknown,
	WMT_Directional,
	WMT_Sphere,
	WMT_Vortex,
};

class FWindMotorBaseParamBase
{
public:
	virtual ~FWindMotorBaseParamBase(){}
	virtual EWindMotorType GetWindMotorType() const{return WMT_Unknown;};

public:
	FVector MotorWorldSpacePos;                  //Min of wind velocity
	FVector WindVelocity;
	float Radius = 1;								//wind drag force
};


struct FWindMotorParamData
{
	TArray<FWindMotorBaseParamBase*> AllWindMotors;
	FVector PlayerWorldPos;
	float MaxVelocity;
	float TexelsPerMeter;
	FWindVelocityTextures* WindVelocityTexturesDoubleBuffer;
};

