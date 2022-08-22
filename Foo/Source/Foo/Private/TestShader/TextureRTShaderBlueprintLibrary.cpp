// Copyright Epic Games, Inc. All Rights Reserved.

#include "TextureRTShaderBlueprintLibrary.h"

#include "Engine/TextureRenderTarget2D.h"


UTextureRTShaderBlueprintLibrary::UTextureRTShaderBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ }


class FMyTextureShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FMyTextureShaderBase, NonVirtual);
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

	FMyTextureShaderBase() {}

	FMyTextureShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		MyColorVal.Bind(Initializer.ParameterMap, TEXT("MyColor"));
		MyTextureVal.Bind(Initializer.ParameterMap, TEXT("MyTexture"));
		MyTextureSampler.Bind(Initializer.ParameterMap, TEXT("MyTextureSampler"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const FLinearColor& MyColor,
		FTexture2DRHIRef InputTexture)
	{
		SetShaderValue(RHICmdList, ShaderRHI, MyColorVal, MyColor);
		SetTextureParameter(RHICmdList,ShaderRHI,MyTextureVal,MyTextureSampler,
			TStaticSamplerState<SF_Trilinear,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI(),
			InputTexture);
	}

private:
	
	LAYOUT_FIELD(FShaderParameter, MyColorVal);
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureVal);
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureSampler);
};

struct FMyTextureVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FMyTextureVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FMyTextureVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
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

class FMyTextureIndexBuffer : public FIndexBuffer
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

class FMyTextureVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyTextureVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyTextureVertex,Position),VET_Float4,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyTextureVertex,UV),VET_Float2,1,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

TGlobalResource<FMyTextureVertexBuffer> g_MyTextureVertexBuffer;
TGlobalResource<FMyTextureIndexBuffer> g_MyTextureIndexBuffer;

class FMyTextureShaderVS : public FMyTextureShaderBase
{
	DECLARE_SHADER_TYPE(FMyTextureShaderVS, Global);
public:

	/** Default constructor. */
	FMyTextureShaderVS() {}

	/** Initialization constructor. */
	FMyTextureShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyTextureShaderBase(Initializer)
	{
	}
};


class FMyTextureShaderPS : public FMyTextureShaderBase
{
	DECLARE_SHADER_TYPE(FMyTextureShaderPS, Global);
public:

	/** Default constructor. */
	FMyTextureShaderPS() {}

	/** Initialization constructor. */
	FMyTextureShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyTextureShaderBase(Initializer)
	{ }
};

IMPLEMENT_SHADER_TYPE(, FMyTextureShaderVS, TEXT("/Plugin/Foo/Private/TestShader/MyTextureShader.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FMyTextureShaderPS, TEXT("/Plugin/Foo/Private/TestShader/MyTextureShader.usf"), TEXT("MainPS"), SF_Pixel)


// IMPLEMENT_SHADER_TYPE(, FMyTextureShaderVS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainVS"), SF_Vertex)
// IMPLEMENT_SHADER_TYPE(, FMyTextureShaderPS, TEXT("/Plugin/Foo/Private/ComputeShader/Debug/WindDebugSamplePlane.usf"), TEXT("MainPS"), SF_Pixel)

static void DrawTextureToRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FLinearColor MyColor,
	FTexture2DRHIRef MyTexture)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("MyPixelShaderPassTest"));
#else
	SCOPED_DRAW_EVENT(RHICmdList, DrawUVDisplacementToRenderTarget_RenderThread);
#endif

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();
	RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, RenderTargetTexture);

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store, OutTextureRenderTargetResource->TextureRHI);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("MyPixelShaderPass"));
	{
		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef< FMyTextureShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FMyTextureShaderPS > PixelShader(GlobalShaderMap);

		FMyTextureVertexDeclaration vertexDeclartion;
		vertexDeclartion.InitRHI();

		// Set the graphic pipeline state.
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = vertexDeclartion.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);

		// Update shader uniform parameters.
		// VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), CompiledCameraModel, DisplacementMapResolution);
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), MyColor,MyTexture);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_MyTextureVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_MyTextureIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}

// static
void UTextureRTShaderBlueprintLibrary::DrawTextureToRenderTarget(
	const UObject* WorldContextObject,
	class UTextureRenderTarget2D* OutputRenderTarget,
	FLinearColor MyColor,
	UTexture* MyTexture
	)
{
	check(IsInGameThread());

	if(!OutputRenderTarget)
		return;

	FTextureRenderTargetResource* TextureRenderTargetReource  = OutputRenderTarget->GameThread_GetRenderTargetResource();
	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();
	FTexture2DRHIRef TextureRHI = MyTexture->Resource->TextureRHI->GetTexture2D();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetReource,FeatureLevel,MyColor,TextureRHI](FRHICommandListImmediate& RHICmdList)
		{
			DrawTextureToRenderTarget_RenderThread(RHICmdList,TextureRenderTargetReource,FeatureLevel,MyColor,TextureRHI);
		}
	);
}
