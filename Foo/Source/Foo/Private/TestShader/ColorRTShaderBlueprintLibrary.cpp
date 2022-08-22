// Copyright Epic Games, Inc. All Rights Reserved.

#include "ColorRTShaderBlueprintLibrary.h"

#include "Engine/TextureRenderTarget2D.h"


UColorRTShaderBlueprintLibrary::UColorRTShaderBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ }


class FMyShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FMyShaderBase, NonVirtual);
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

	FMyShaderBase() {}

	FMyShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		MyColorVal.Bind(Initializer.ParameterMap, TEXT("MyColor"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const FLinearColor& MyColor)
	{
		SetShaderValue(RHICmdList, ShaderRHI, MyColorVal, MyColor);
	}

private:
	
	LAYOUT_FIELD(FShaderParameter, MyColorVal);
};

struct FMyVertex
{
	FVector4 Position;
};

class FMyVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FMyVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(4);
		Vertices[0].Position = FVector4(-1,1,0,1);
		Vertices[1].Position = FVector4(1,1,0,1);
		Vertices[2].Position = FVector4(-1,-1,0,1);
		Vertices[3].Position = FVector4(1,-1,0,1);
		FRHIResourceCreateInfo CreateInfo(&Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(),BUF_Static,CreateInfo);
	}
};

class FMyIndexBuffer : public FIndexBuffer
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

class FMyVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyVertex,Position),VET_Float4,0,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

TGlobalResource<FMyVertexBuffer> g_MyVertexBuffer;
TGlobalResource<FMyIndexBuffer> g_MyIndexBuffer;

class FMyShaderVS : public FMyShaderBase
{
	DECLARE_SHADER_TYPE(FMyShaderVS, Global);
public:

	/** Default constructor. */
	FMyShaderVS() {}

	/** Initialization constructor. */
	FMyShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyShaderBase(Initializer)
	{
	}
};


class FMyShaderPS : public FMyShaderBase
{
	DECLARE_SHADER_TYPE(FMyShaderPS, Global);
public:

	/** Default constructor. */
	FMyShaderPS() {}

	/** Initialization constructor. */
	FMyShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyShaderBase(Initializer)
	{ }
};

IMPLEMENT_SHADER_TYPE(, FMyShaderVS, TEXT("/Plugin/Foo/Private/TestShader/MyColorShader.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FMyShaderPS, TEXT("/Plugin/Foo/Private/TestShader/MyColorShader.usf"), TEXT("MainPS"), SF_Pixel)

static void DrawColorToRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FLinearColor MyColor)
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
		TShaderMapRef< FMyShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FMyShaderPS > PixelShader(GlobalShaderMap);

		FMyVertexDeclaration vertexDeclartion;
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
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), MyColor);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_MyVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_MyIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}

// static
void UColorRTShaderBlueprintLibrary::DrawSimpleColorToRenderTarget(
	const UObject* WorldContextObject,
	class UTextureRenderTarget2D* OutputRenderTarget,
	FLinearColor MyColor)
{
	check(IsInGameThread());

	if(!OutputRenderTarget)
		return;

	FTextureRenderTargetResource* TextureRenderTargetReource  = OutputRenderTarget->GameThread_GetRenderTargetResource();
	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetReource,FeatureLevel,MyColor](FRHICommandListImmediate& RHICmdList)
		{
			DrawColorToRenderTarget_RenderThread(RHICmdList,TextureRenderTargetReource,FeatureLevel,MyColor);
		}
	);
}
