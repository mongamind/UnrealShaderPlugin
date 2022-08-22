// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeShaderBlueprintLibrary.h"

#include "Engine/TextureRenderTarget2D.h"


UComputeShaderBlueprintLibrary::UComputeShaderBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ }


IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FMyComputeShaderStructData, "FMyCSUniform");

class FMyComputeShaderShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FMyComputeShaderShader, Global);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	}

	FMyComputeShaderShader() {}

	FMyComputeShaderShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		OutTexture.Bind(Initializer.ParameterMap,TEXT("OutTexture"));
	}

	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		FRHIUnorderedAccessView* InOutUAV,
		const FMyComputeShaderCPPData &ShaderStructData)
	{

		FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
		if(OutTexture.IsBound())
		{
			RHICmdList.SetUAVParameter(ComputeShaderRHI,OutTexture.GetBaseIndex(),InOutUAV);

			FMyComputeShaderStructData UniformData;
			UniformData.Color1 = ShaderStructData.Color1;
			UniformData.Color2 = ShaderStructData.Color2;
			UniformData.Color3 = ShaderStructData.Color3;
			UniformData.Color4 = ShaderStructData.Color4;
			UniformData.ColorIndex = ShaderStructData.ColorIndex;
			UniformData.GlobalTime = ShaderStructData.GlobalTime;

			SetUniformBufferParameterImmediate(RHICmdList,ComputeShaderRHI,GetUniformBufferParameter<FMyComputeShaderStructData>(), UniformData);
		}
	}

	void UnbindBuffers(FRHICommandListImmediate& RHICmdList)
	{
		FRHIComputeShader* ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
		if(OutTexture.IsBound())
		{
			RHICmdList.SetUAVParameter(ComputeShaderRHI,OutTexture.GetBaseIndex(),nullptr);
		}
	}

private:
	LAYOUT_FIELD(FShaderResourceParameter,OutTexture);
};


class FMyTestShaderBase : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FMyTestShaderBase, NonVirtual);
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

	FMyTestShaderBase() {}

	FMyTestShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		MyTextureVal.Bind(Initializer.ParameterMap, TEXT("MyTexture"));
		MyTextureSampler.Bind(Initializer.ParameterMap, TEXT("MyTextureSampler"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		FTexture2DRHIRef InputTexture)
	{
		SetTextureParameter(RHICmdList,ShaderRHI,MyTextureVal,MyTextureSampler,
			TStaticSamplerState<SF_Trilinear,AM_Clamp,AM_Clamp,AM_Clamp>::GetRHI(),
			InputTexture);

	}

private:
	
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureVal);
	LAYOUT_FIELD(FShaderResourceParameter, MyTextureSampler);
};

struct FMyTestVertex
{
	FVector4 Position;
	FVector2D UV;
};

class FMyTestVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FMyTestVertex,VERTEXBUFFER_ALIGNMENT> Vertices;
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

class FMyTestIndexBuffer : public FIndexBuffer
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

class FMyTestVertexDeclaration : public  FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyTestVertex);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyTestVertex,Position),VET_Float4,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(FMyTestVertex,UV),VET_Float2,1,Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual  void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

TGlobalResource<FMyTestVertexBuffer> g_MyUniformVertexBuffer;
TGlobalResource<FMyTestIndexBuffer> g_MyUniformIndexBuffer;

class FMyTestShaderVS : public FMyTestShaderBase
{
	DECLARE_SHADER_TYPE(FMyTestShaderVS, Global);
public:

	/** Default constructor. */
	FMyTestShaderVS() {}

	/** Initialization constructor. */
	FMyTestShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyTestShaderBase(Initializer)
	{
	}
};


class FMyTestShaderPS : public FMyTestShaderBase
{
	DECLARE_SHADER_TYPE(FMyTestShaderPS, Global);
public:

	/** Default constructor. */
	FMyTestShaderPS() {}

	/** Initialization constructor. */
	FMyTestShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyTestShaderBase(Initializer)
	{ }
};

IMPLEMENT_SHADER_TYPE(, FMyComputeShaderShader, TEXT("/Plugin/Foo/Private/TestShader/MyComputeShader.usf"), TEXT("MainCS"), SF_Compute)
IMPLEMENT_SHADER_TYPE(, FMyTestShaderVS, TEXT("/Plugin/Foo/Private/TestShader/MyTextureShader_ForComputeShaderTest.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FMyTestShaderPS, TEXT("/Plugin/Foo/Private/TestShader/MyTextureShader_ForComputeShaderTest.usf"), TEXT("MainPS"), SF_Pixel)

static void DrawRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
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
		TShaderMapRef< FMyTestShaderVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FMyTestShaderPS > PixelShader(GlobalShaderMap);

		FMyTestVertexDeclaration vertexDeclartion;
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
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), MyTexture);

		// Draw grid.
		RHICmdList.SetStreamSource(0,g_MyUniformVertexBuffer.VertexBufferRHI,0);
		RHICmdList.DrawIndexedPrimitive(g_MyUniformIndexBuffer.IndexBufferRHI,0,0,4,0,2,1);
	}
	RHICmdList.EndRenderPass();
}


static void UseComputeShaderToDraw_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	const FMyComputeShaderCPPData& UniformStructData)
{
	check(IsInRenderingThread())

	TShaderMapRef<FMyComputeShaderShader> ComputeShader(GetGlobalShaderMap(FeatureLevel));
	RHICmdList.SetComputeShader(ComputeShader.GetComputeShader());

	int32 SizeX = OutTextureRenderTargetResource->GetSizeX();
	int32 SizeY = OutTextureRenderTargetResource->GetSizeY();

	FRHIResourceCreateInfo CreateInfo;
	FTexture2DRHIRef Texture = RHICreateTexture2D(SizeX,SizeY,PF_A32B32G32R32F,1,1,TexCreate_ShaderResource | TexCreate_UAV,CreateInfo);
	FUnorderedAccessViewRHIRef TextureUAV = RHICreateUnorderedAccessView(Texture);
	ComputeShader->SetParameters(RHICmdList,TextureUAV,UniformStructData);
	DispatchComputeShader(RHICmdList,ComputeShader,SizeX / 32,SizeY / 32,1);
	ComputeShader->UnbindBuffers(RHICmdList);

	DrawRenderTarget_RenderThread(RHICmdList,OutTextureRenderTargetResource,FeatureLevel,Texture);
}

// static
void UComputeShaderBlueprintLibrary::UseComputeShaderToDraw(
	const UObject* WorldContextObject,
	class UTextureRenderTarget2D* OutputRenderTarget,
	const FMyComputeShaderCPPData& UniformStructData
	)
{
	check(IsInGameThread());

	if(!OutputRenderTarget)
		return;

	FTextureRenderTargetResource* TextureRenderTargetReource  = OutputRenderTarget->GameThread_GetRenderTargetResource();
	const UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetReource,FeatureLevel,&UniformStructData](FRHICommandListImmediate& RHICmdList)
		{
			UseComputeShaderToDraw_RenderThread(RHICmdList,TextureRenderTargetReource,FeatureLevel,UniformStructData);
		}
	);
}
