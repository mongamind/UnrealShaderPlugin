// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

///////////////////////////////////////////////////////////////////////////////////////////


class FWindExportShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FWindExportShaderBase, NonVirtual);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		// OutEnvironment.SetDefine(TEXT("GRID_SUBDIVISION_X"), kGridSubdivisionX);
		// OutEnvironment.SetDefine(TEXT("GRID_SUBDIVISION_Y"), kGridSubdivisionY);
	}

	FWindExportShaderBase() {}

	FWindExportShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		WindDiffusionXAxisTexture.Bind(Initializer.ParameterMap, TEXT("InWindDiffusionXAxisTexture"));
		WindDiffusionYAxisTexture.Bind(Initializer.ParameterMap, TEXT("InWindDiffusionYAxisTexture"));
		WindDiffusionZAxisTexture.Bind(Initializer.ParameterMap, TEXT("InWindDiffusionZAxisTexture"));
		// MaxWindVolecity.Bind(Initializer.ParameterMap, TEXT("InMaxWindVolecity"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		FShaderResourceViewRHIRef WindDiffusionXAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionYAxisTexture_SRV,
		FShaderResourceViewRHIRef WindDiffusionZAxisTexture_SRV/*,
		float InMaxWindVolecity*/)
	{
		SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionXAxisTexture,WindDiffusionXAxisTexture_SRV);
		SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionYAxisTexture,WindDiffusionYAxisTexture_SRV);
		SetSRVParameter(RHICmdList,ShaderRHI,WindDiffusionZAxisTexture,WindDiffusionZAxisTexture_SRV);

		// SetShaderValue(RHICmdList,ShaderRHI,MaxWindVolecity,InMaxWindVolecity);
	}

private:
	
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionXAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionYAxisTexture);
	LAYOUT_FIELD(FShaderResourceParameter, WindDiffusionZAxisTexture);
	// LAYOUT_FIELD(FShaderParameter, MaxWindVolecity);
};

struct FWindExportVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FWindExportVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FWindExportVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
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

class FWindExportIndexBuffer : public FIndexBuffer
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

class FWindExportVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FWindExportVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FWindExportVertex,Position),VET_Float4,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FWindExportVertex,UV),VET_Float2,1,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

class FWindExportShaderVS : public FWindExportShaderBase
{
	DECLARE_SHADER_TYPE(FWindExportShaderVS, Global);
public:

	/** Default constructor. */
	FWindExportShaderVS() {}

	/** Initialization constructor. */
	FWindExportShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindExportShaderBase(Initializer)
	{
	}
};


class FWindExportShaderPS : public FWindExportShaderBase
{
	DECLARE_SHADER_TYPE(FWindExportShaderPS, Global);
public:

	/** Default constructor. */
	FWindExportShaderPS() {}

	/** Initialization constructor. */
	FWindExportShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWindExportShaderBase(Initializer)
	{ }
};

