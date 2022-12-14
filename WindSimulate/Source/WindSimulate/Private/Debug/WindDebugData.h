// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Diffusion/WindDiffusionData.h"

struct FDebugShaderVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FWindDebugVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FDebugShaderVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(4);
		Vertices[0].Position = FVector4(-1,1,0,1);
		Vertices[1].Position = FVector4(1,1,0,1);
		Vertices[2].Position = FVector4(-1,-1,0,1);
		Vertices[3].Position = FVector4(1,-1,0,1);

		Vertices[0].UV = FVector2D(0,1);
		Vertices[1].UV = FVector2D(1,1);
		Vertices[2].UV = FVector2D(0,0);
		Vertices[3].UV = FVector2D(1,0);
		
		FRHIResourceCreateInfo CreateInfo(&Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(),BUF_Static,CreateInfo);
	}
};

class FWindDebugIndexBuffer : public FIndexBuffer
{
public:
	void InitRHI() override
	{
		const uint16 Indices[] = {0,1,2,2,1,3};
		TResourceArray<uint16,INDEXBUFFER_ALIGNMENT> IndexBuffer;
		uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer.AddUninitialized(NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(),Indices,NumIndices * sizeof(uint16));

		FRHIResourceCreateInfo CreateInfo(&IndexBuffer);
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16),IndexBuffer.GetResourceDataSize(),BUF_Static,CreateInfo);
	}
};

class FWindDebugVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FDebugShaderVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FDebugShaderVertex,Position),VET_Float4,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FDebugShaderVertex,UV),VET_Float2,1,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};


class FWindDebugShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FWindDebugShaderBase, NonVirtual);
public:

	FWindDebugShaderBase() {}

	FWindDebugShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		float InPadding,
		float InMaxWindVelocity,
		FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV);

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList);

private:
	LAYOUT_FIELD(FShaderParameter, Padding);
	LAYOUT_FIELD(FShaderParameter, MaxWindVelocity);
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionZAxisTexture);
};

class FWindDebugXYZAxisShaderVS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugXYZAxisShaderVS, Global);
public:

	/** Default constructor. */
	FWindDebugXYZAxisShaderVS() {}

	/** Initialization constructor. */
	FWindDebugXYZAxisShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{
	}
};


class FWindDebugXYZAxisShaderPS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugXYZAxisShaderPS, Global);
public:

	/** Default constructor. */
	FWindDebugXYZAxisShaderPS() {}

	/** Initialization constructor. */
	FWindDebugXYZAxisShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{ }
};

class FWindDebugTex3DShaderVS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugTex3DShaderVS, Global);
public:

	/** Default constructor. */
	FWindDebugTex3DShaderVS() {}

	/** Initialization constructor. */
	FWindDebugTex3DShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{
	}
};


class FWindDebugTex3DShaderPS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugTex3DShaderPS, Global);
public:

	/** Default constructor. */
	FWindDebugTex3DShaderPS() {}

	/** Initialization constructor. */
	FWindDebugTex3DShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{ }
};

