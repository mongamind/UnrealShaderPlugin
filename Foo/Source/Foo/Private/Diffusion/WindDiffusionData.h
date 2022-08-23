// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SharedPointer.h"
// #include "WindDiffusionData.generated.h"

struct FWindSetting
{
	FVector InPosDelta;
	
	float MaxWindVelocity;                   //Max of wind velocity.
	float OriginalAlpha = 0.9f;
	float BetaNearAdd = 0.14f;
};


struct FWindVelocityTextures
{
public:
	void Init(EPixelFormat InFormat, uint32 InX, uint32 InY, uint32 InZ);

	void Release();

	

	void Barrier(FRHICommandList& RHICmdList, EResourceTransitionAccess InBarrierMode, EResourceTransitionPipeline InPipeline,FRHIComputeFence* Fence) const;

	void ClearCurXAxisUAVToBlack(FRHICommandList& RHICmdList) const;
	void ClearCurYAxisUAVToBlack(FRHICommandList& RHICmdList) const;
	void ClearCurZAxisUAVToBlack(FRHICommandList& RHICmdList) const;

	void SwapXAxisBuff(){IndexCurXAxisTexture = (IndexCurXAxisTexture + 1) % 2;}
	void SwapYAxisBuff(){IndexCurYAxisTexture = (IndexCurYAxisTexture + 1) % 2;}
	void SwapZAxisBuff(){IndexCurZAxisTexture = (IndexCurZAxisTexture + 1) % 2;}

	void SwapAllBuffer()
	{
		SwapXAxisBuff();
		SwapYAxisBuff();
		SwapZAxisBuff();
	}
	

	FUnorderedAccessViewRHIRef GetCurXAxisUAV()const {return WindDiffusionXAxisTexture_UAV[IndexCurXAxisTexture];}
	FShaderResourceViewRHIRef GetCurXAxisSRV()const {return WindDiffusionXAxisTexture_SRV[(IndexCurXAxisTexture + 1) % 2];}

	FUnorderedAccessViewRHIRef GetCurYAxisUAV()const {return WindDiffusionYAxisTexture_UAV[IndexCurYAxisTexture];}
	FShaderResourceViewRHIRef GetCurYAxisSRV()const {return WindDiffusionYAxisTexture_SRV[(IndexCurYAxisTexture + 1) % 2];}

	FUnorderedAccessViewRHIRef GetCurZAxisUAV()const {return WindDiffusionZAxisTexture_UAV[IndexCurZAxisTexture];}
	FShaderResourceViewRHIRef GetCurZAxisSRV()const {return WindDiffusionZAxisTexture_SRV[(IndexCurZAxisTexture + 1) % 2];}

	FTexture3DRHIRef WindDiffusionXAxisTexture[2];
	FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV[2];
	FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV[2];
	int IndexCurXAxisTexture = 0;


	FTexture3DRHIRef WindDiffusionYAxisTexture[2];
	FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV[2];
	FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV[2];
	int IndexCurYAxisTexture = 0;


	FTexture3DRHIRef WindDiffusionZAxisTexture[2];
	FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV[2];
	FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV[2];
	int IndexCurZAxisTexture = 0;

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;

};

// struct FWindVelocityTexturesDoubleBuffer
// {
// 	//double buffer.
// 	FWindVelocityTextures VelocityTextures[2];
// 	int IndexCurVelocityTexture = 0;
//
// 	void InitVelocityTextures();
// 	void ReleaseVelocityTextures();
//
// 	void SwapBuffer()
// 	{
// 		IndexCurVelocityTexture = (IndexCurVelocityTexture + 1) % 2;
// 	}
//
// 	const FWindVelocityTextures& GetCurVelocityTextures()const 
// 	{
// 		return VelocityTextures[IndexCurVelocityTexture];
// 	}
//
// 	const FWindVelocityTextures& GetLastVelocityTextures()const 
// 	{
// 		return VelocityTextures[(IndexCurVelocityTexture + 1) % 2];
// 	}
// };

struct FWindVelocityFieldDataParam
{
	FWindSetting WindSetting;

	//double buffer.
	FWindVelocityTextures* WindVelocityTexturesDoubleBuffer;
};

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FWindVelocityFieldData, )
	SHADER_PARAMETER(FVector,InPosDelta)
	SHADER_PARAMETER(float,InMaxWindVelocity)
	SHADER_PARAMETER(float,OriginalAlpha)
	SHADER_PARAMETER(float,BetaNearAdd)
END_GLOBAL_SHADER_PARAMETER_STRUCT()


class FWindDiffuse2DArrayResource : public FTextureResource
{
public:
	FWindDiffuse2DArrayResource(uint32 InSizeX, uint32 InSizeY, uint32 InSizeZ, EPixelFormat InFormat, uint32 InNumMips, bool InNeedUAV)
		: SizeX(InSizeX)
		, SizeY(InSizeY)
		, SizeZ(InSizeZ)
		, Format(InFormat)
		, NumMips(InNumMips)
		, CreateUAV(InNeedUAV)
	{}

	virtual uint32 GetSizeX() const override;

	virtual uint32 GetSizeY() const override;

	virtual uint32 GetSizeZ() const override;

	/** Called when the resource is initialized. This is only called by the rendering thread. */
	virtual void InitRHI() override;

	virtual void ReleaseRHI() override;

	FUnorderedAccessViewRHIRef TextureUAV;

private:
	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;
	EPixelFormat Format;
	uint32 NumMips;
	bool CreateUAV;
};

class FWindOneAxisDiffusionShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FWindOneAxisDiffusionShader, Global);
public:

	FWindOneAxisDiffusionShader() {}

	FWindOneAxisDiffusionShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const FWindSetting& WindSetting,
		FUnorderedAccessViewRHIRef WindDiffusionOneAxisTexture_UAV,
		FShaderResourceViewRHIRef WindDiffusionOneAxisTexture_SRV);

	void UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionOneAxisTexture_UAV,
		FRHIComputeFence* Fence);

	void UnsetParameters(
		FRHICommandList& RHICmdList,
		EResourceTransitionAccess TransitionAccess,
		EResourceTransitionPipeline TransitionPipeline,
		FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV,
		FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV,
		FRHIComputeFence* Fence);

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList);

private:
	
	LAYOUT_FIELD(FShaderResourceParameter, InTexture);
	LAYOUT_FIELD(FShaderResourceParameter,OutTexture);
};
