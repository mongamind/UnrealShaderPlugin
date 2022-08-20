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
	
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		float InPadding,
		FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV);

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList);

private:
	LAYOUT_FIELD(FShaderParameter, MyColor);
	// LAYOUT_FIELD(FShaderParameter, Padding);
	// LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionXAxisTexture);
	// LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionYAxisTexture);
	// LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionZAxisTexture);
};

class FWindDebugShaderVS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugShaderVS, Global);
public:

	/** Default constructor. */
	FWindDebugShaderVS() {}

	/** Initialization constructor. */
	FWindDebugShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{
	}
};


class FWindDebugShaderPS : public FWindDebugShaderBase
{
	DECLARE_SHADER_TYPE(FWindDebugShaderPS, Global);
public:

	/** Default constructor. */
	FWindDebugShaderPS() {}

	/** Initialization constructor. */
	FWindDebugShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindDebugShaderBase(Initializer)
	{ }
};
