// Copyright Epic Games, Inc. All Rights Reserved.

#include "Export/ExportData.h"
#include "RHIDefinitions.h"

IMPLEMENT_SHADER_TYPE(, FWindExportShaderVS, TEXT("/Plugin/WindSimulate/Private/ComputeShader/Export/WindMergeToRT.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FWindExportShaderPS, TEXT("/Plugin/WindSimulate/Private/ComputeShader/Export/WindMergeToRT.usf"), TEXT("MainPS"), SF_Pixel)
