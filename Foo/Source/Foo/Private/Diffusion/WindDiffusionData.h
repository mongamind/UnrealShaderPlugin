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
	
	float InMaxWindVelocity;                   //Max of wind velocity.
	
	float OriginalAlpha = 0.9f;
	float BetaNearAdd = 0.14f;
};


struct FWindVelocityTextures
{
public:
	void Init(EPixelFormat InFormat, uint32 InX, uint32 InY, uint32 InZ);

	void Release();

	void ClearUAVToBlack(FRHICommandList& RHICmdList) const;

	void Barrier(FRHICommandList& RHICmdList, EResourceTransitionAccess InBarrierMode, EResourceTransitionPipeline InPipeline) const;

	FTexture3DRHIRef WindDiffusionXAxisTexture;
	FUnorderedAccessViewRHIRef WindDiffusionXAxisTexture_UAV;
	FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV;

	FTexture3DRHIRef WindDiffusionYAxisTexture;
	FUnorderedAccessViewRHIRef WindDiffusionYAxisTexture_UAV;
	FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV;

	FTexture3DRHIRef WindDiffusionZAxisTexture;
	FUnorderedAccessViewRHIRef WindDiffusionZAxisTexture_UAV;
	FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV;

	uint32 SizeX;
	uint32 SizeY;
	uint32 SizeZ;

};

struct FWindVelocityTexturesDoubleBuffer
{
	//double buffer.
	FWindVelocityTextures VelocityTextures[2];
	int IndexCurVelocityTexture = 0;

	void InitVelocityTextures();
	void ReleaseVelocityTextures();

	void SwapBuffer()
	{
		IndexCurVelocityTexture = (IndexCurVelocityTexture + 1) % 2;
	}

	const FWindVelocityTextures& GetCurVelocityTextures()const 
	{
		return VelocityTextures[IndexCurVelocityTexture];
	}

	const FWindVelocityTextures& GetLastVelocityTextures()const 
	{
		return VelocityTextures[(IndexCurVelocityTexture + 1) % 2];
	}
};

struct FWindVelocityFieldDataParam
{
	FWindSetting WindSetting;

	//double buffer.
	FWindVelocityTexturesDoubleBuffer* WindVelocityTexturesDoubleBuffer;
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

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList);

private:
	LAYOUT_FIELD(FShaderResourceParameter, InTexture);
	
	LAYOUT_FIELD(FShaderResourceParameter,OutTexture);
};
