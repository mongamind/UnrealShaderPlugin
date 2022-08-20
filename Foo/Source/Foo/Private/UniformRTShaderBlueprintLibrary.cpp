// Copyright Epic Games, Inc. All Rights Reserved.

#include "UniformRTShaderBlueprintLibrary.h"

#include "Engine/TextureRenderTarget2D.h"


UUniformRTShaderBlueprintLibrary::UUniformRTShaderBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ }


IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FMyUniformStructData, "FMyUniform");

class FMyUniformShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FMyUniformShaderBase, NonVirtual);
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

	FMyUniformShaderBase() {}

	FMyUniformShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
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
		FTexture2DRHIRef InputTexture,
		const FMyShaderStructData &ShaderStructData)
	{
		SetShaderValue(RHICmdList, ShaderRHI, MyColorVal, MyColor);
		SetTextureParameter(RHICmdList,ShaderRHI,MyTextureVal,MyTextureSampler,
			TStaticSamplerState<SF_Trilinear,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI(),
			InputTexture);

		FMyUniformStructData UniformData;
		UniformData.Color1 = ShaderStructData.Color1;
		UniformData.Color2 = ShaderStructData.Color2;
		UniformData.Color3 = ShaderStructData.Color3;
		UniformData.Color4 = ShaderStructData.Color4;
		UniformData.ColorIndex = ShaderStructData.ColorIndex;
		// SetUniformBufferParameterImmediate(RHICmdList,RHICmdList.GetBoundPixelShader(),GetUniformBufferParameter<FMyUniformStructData>(),UniformData);
		SetUniformBufferParameterImmediate(RHICmdList,ShaderRHI,GetUniformBufferParameter<FMyUniformStructData>(), UniformData);
	}

private:
	
	LAYOUT_FIELD(FShaderParameter, MyColorVal);
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureVal);
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureSampler);
};

struct FMyUnformVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FMyUniformVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FMyUnformVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
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

class FMyUniformIndexBuffer : public FIndexBuffer
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

class FMyUniformVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyUnformVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyUnformVertex,Position),VET_Float4,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyUnformVertex,UV),VET_Float2,1,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

TGlobalResource<FMyUniformVertexBuffer> g_MyTestVertexBuffer;
TGlobalResource<FMyUniformIndexBuffer> g_MyTestIndexBuffer;

class FMyUniformShaderVS : public FMyUniformShaderBase
{
	DECLARE_SHADER_TYPE(FMyUniformShaderVS, Global);
public:

	/** Default constructor. */
	FMyUniformShaderVS() {}

	/** Initialization constructor. */
	FMyUniformShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyUniformShaderBase(Initializer)
	{
	}
};


class FMyUniformShaderPS : public FMyUniformShaderBase
{
	DECLARE_SHADER_TYPE(FMyUniformShaderPS, Global);
public:

	/** Default constructor. */
	FMyUniformShaderPS() {}

	/** Initialization constructor. */
	FMyUniformShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyUniformShaderBase(Initializer)
	{ }
};

IMPLEMENT_SHADER_TYPE(, FMyUniformShaderVS, TEXT("/Plugin/Foo/Private/MyUniformShader.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FMyUniformShaderPS, TEXT("/Plugin/Foo/Private/MyUniformShader.usf"), TEXT("MainPS"), SF_Pixel)

static void DrawUniformToRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FLinearColor MyColor,
	FTexture2DRHIRef MyTexture,
	const FMyShaderStructData& UniformStructData)
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
		TShaderMapRef< FMyUniformShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FMyUniformShaderPS > PixelShader(GlobalShaderMap);

		FMyUniformVertexDeclaration vertexDeclartion;
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
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), MyColor,MyTexture,UniformStructData);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_MyTestVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_MyTestIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}

// static
void UUniformRTShaderBlueprintLibrary::DrawUniformToRenderTarget(
	const UObject* WorldContextObject,
	class UTextureRenderTarget2D* OutputRenderTarget,
	FLinearColor MyColor,
	UTexture* MyTexture,
	const FMyShaderStructData& UniformStructData
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
		[TextureRenderTargetReource,FeatureLevel,MyColor,TextureRHI,&UniformStructData](FRHICommandListImmediate& RHICmdList)
		{
			DrawUniformToRenderTarget_RenderThread(RHICmdList,TextureRenderTargetReource,FeatureLevel,MyColor,TextureRHI,UniformStructData);
		}
	);
}
