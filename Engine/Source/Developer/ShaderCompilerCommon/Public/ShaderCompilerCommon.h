// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "ShaderCore.h"
#include "CrossCompilerCommon.h"


/**
 * This function looks for resources specified in ResourceTableMap in the 
 * parameter map, adds them to the resource table, and removes them from the
 * parameter map. If a resource is used from a currently unmapped uniform 
 * buffer we allocate a slot for it from UsedUniformBufferSlots.
 */
extern SHADERCOMPILERCOMMON_API void BuildResourceTableMapping(
		const TMap<FString,FResourceTableEntry>& ResourceTableMap,
		const TMap<FString,uint32>& ResourceTableLayoutHashes,
		TBitArray<>& UsedUniformBufferSlots,
		FShaderParameterMap& ParameterMap,
		FShaderCompilerResourceTable& OutSRT
	);

/**
 * Builds a token stream out of the resource map. The resource map is one
 * of the arrays generated by BuildResourceTableMapping. The token stream
 * is used at runtime to gather resources from tables and bind them to the
 * appropriate slots.
 */
extern SHADERCOMPILERCOMMON_API void BuildResourceTableTokenStream(
	const TArray<uint32>& InResourceMap,
	int32 MaxBoundResourceTable,
	TArray<uint32>& OutTokenStream
	);

// Finds the number of used uniform buffers in a resource map
extern SHADERCOMPILERCOMMON_API int16 GetNumUniformBuffersUsed(const FShaderCompilerResourceTable& InSRT);


// This function goes through the shader source and converts
//		static struct
//		{
//		TYPE Member;
//		[...]
//		} UniformBuffer = { [...] };
//		
//		TYPE Function([...])
//		{
//			[...]UniformBuffer.Member[...]
//		}
//
//into
//
//		/*atic struct
//		{
//		TYPE Member;
//		[...]
//		} UniformBuffer = { [...] */
//		
//		TYPE Function([...])
//		{
//		[...]UniformBuffer_Member[...]
//		}
//
// If instanced stereo is not enabled, replace ResolvedView with View.
extern SHADERCOMPILERCOMMON_API bool RemoveUniformBuffersFromSource(FString& SourceCode);
extern SHADERCOMPILERCOMMON_API bool RemoveUnusedOutputs(FString& InOutSourceCode, const TArray<FString>& InUsedOutputs, FString& InOutEntryPoint, TArray<FString>& OutErrors);

extern SHADERCOMPILERCOMMON_API bool RemoveUnusedInputs(FString& InOutSourceCode, const TArray<FString>& InUsedInputs, FString& InOutEntryPoint, TArray<FString>& OutErrors);

// Cross compiler support/common functionality
namespace CrossCompiler
{
	extern SHADERCOMPILERCOMMON_API FString CreateBatchFileContents(
		const FString& ShaderFile,
		const FString& OutputFile,
		uint32 Frequency,
		const FString& EntryPoint,
		const FString& VersionSwitch,
		uint32 CCFlags,
		const FString& ExtraArguments = TEXT(""));

	extern SHADERCOMPILERCOMMON_API void ParseHlslccError(TArray<FShaderCompilerError>& OutErrors, const FString& InLine);

	struct SHADERCOMPILERCOMMON_API FHlslccHeader
	{
		FHlslccHeader();

		bool Read(const ANSICHAR*& ShaderSource, int32 SourceLen);

		struct FInOut
		{
			FString Type;
			int32 Index;
			int32 ArrayCount;
			FString Name;
		};

		struct FAttribute
		{
			int32 Index;
			FString Name;
		};

		struct FPackedGlobal
		{
			ANSICHAR PackedType;
			FString Name;
			int32 Offset;
			int32 Count;
		};

		//struct FUniform
		//{
		//};

		struct FPackedUB
		{
			FAttribute Attribute;
			struct FMember
			{
				FString Name;
				int32 Offset;
				int32 Count;
			};
			TArray<FMember> Members;
		};

		struct FPackedUBCopy
		{
			int32 SourceUB;
			int32 SourceOffset;
			int32 DestUB;
			ANSICHAR DestPackedType;
			int32 DestOffset;
			int32 Count;
		};

		struct FSampler
		{
			FString Name;
			int32 Offset;
			int32 Count;
			TArray<FString> SamplerStates;
		};

		struct FUAV
		{
			FString Name;
			int32 Offset;
			int32 Count;
		};

		FString Name;
		TArray<FInOut> Inputs;
		TArray<FInOut> Outputs;
		TArray<FAttribute> UniformBlocks;
		//TArray<FUniform> Uniforms;
		TArray<FPackedGlobal> PackedGlobals;
		TArray<FPackedUB> PackedUBs;
		TArray<FPackedUBCopy> PackedUBCopies;
		TArray<FPackedUBCopy> PackedUBGlobalCopies;
		TArray<FSampler> Samplers;
		TArray<FUAV> UAVs;
		TArray<FAttribute> SamplerStates;
		uint32 NumThreads[3];

		static bool ReadInOut(const ANSICHAR*& ShaderSource, TArray<FInOut>& OutAttributes);
		static bool ReadCopies(const ANSICHAR*& ShaderSource, bool bGlobals, TArray<FPackedUBCopy>& OutCopies);
	};

	extern SHADERCOMPILERCOMMON_API const TCHAR* GetFrequencyName(EShaderFrequency Frequency);
}
