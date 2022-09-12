// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Float16ColorArrayWrapper.generated.h"

/**
 * 
 */
USTRUCT()
struct WINDSIMULATE_API FFloat16ColorArrayWrapper
{
	GENERATED_BODY()

public:

	TArray<FFloat16Color> Array;
	int32 TextureWidth;
	int32 TextureHeight;
	bool bIsSuccessed = false;

	FORCEINLINE int32 Num() const
	{
		return Array.Num();
	}

	FORCEINLINE int GetIndex(const int32 xIndex,const int32 yIndex) const
	{
		return xIndex + yIndex * TextureWidth;
	}

	FORCEINLINE float R(const int32 index) const
	{
		check(index >= 0 && index < Array.Num());
		return static_cast<float>(Array[index].R);
	}

	FORCEINLINE float G(const int32 index) const
	{
		check(index >= 0 && index < Array.Num());
		return static_cast<float>(Array[index].G);
	}

	FORCEINLINE float B(const int32 index) const
	{
		check(index >= 0 && index < Array.Num());
		return static_cast<float>(Array[index].B);
	}

	FORCEINLINE float A(const int32 index) const
	{
		check(index >= 0 && index < Array.Num());
		return static_cast<float>(Array[index].A);
	}

	FORCEINLINE FVector Color(const int32 index) const
	{
		check(index >= 0 && index < Array.Num());
		const FFloat16Color& Color = Array[index];
		return FVector(static_cast<float>(Color.R),static_cast<float>(Color.G),static_cast<float>(Color.B));
	}

	FORCEINLINE operator TArray<FFloat16Color>&()
	{
		return Array;
	}

	FORCEINLINE operator const TArray<FFloat16Color>& () const
	{
		return Array;
	}

	bool ReadPixelsFromRenderTarget2D(const UTextureRenderTarget2D* const RenderTarget2D)
	{
		bIsSuccessed = false;

		if(IsValid(RenderTarget2D))
		{
			FTextureRenderTarget2DResource* textureResource = static_cast<FTextureRenderTarget2DResource*>(RenderTarget2D->Resource);
			if(textureResource != nullptr)
			{
				if (textureResource->ReadFloat16Pixels(Array, ECubeFace::CubeFace_MAX))
				{
					TextureWidth = textureResource->GetSizeX();
					TextureHeight = textureResource->GetSizeY();

					bIsSuccessed = true;
				}
			}
		}

		return bIsSuccessed;	
	}
};
