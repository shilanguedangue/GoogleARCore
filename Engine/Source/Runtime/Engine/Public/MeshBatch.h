// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MaterialShared.h"
#include "PrimitiveUniformShaderParameters.h"

class FLightCacheInterface;
/**
 * A batch mesh element definition.
 */
struct FMeshBatchElement
{
	/** Primitive uniform buffer to use for rendering. */
	const TUniformBuffer<FPrimitiveUniformShaderParameters>* PrimitiveUniformBufferResource;

	/** Used for lifetime management of a temporary uniform buffer, can be NULL. */
	TUniformBufferRef<FPrimitiveUniformShaderParameters> PrimitiveUniformBuffer;

	const FIndexBuffer* IndexBuffer;
	uint32 FirstIndex;
	uint32 NumPrimitives;
	uint32 NumInstances;
	uint32 MinVertexIndex;
	uint32 MaxVertexIndex;
	int32 UserIndex;
	const void* UserData;
	float MinScreenSize;
	float MaxScreenSize;

	/** 
	 *	DynamicIndexData - pointer to user memory containing the index data.
	 *	Used for rendering dynamic data directly.
	 */
	const void* DynamicIndexData;
	uint16 DynamicIndexStride;
	
	FMeshBatchElement()
	:	PrimitiveUniformBufferResource(NULL)
	,	IndexBuffer(NULL)
	,	NumInstances(1)
	,	UserIndex(-1)
	,	UserData(NULL)
	,	MinScreenSize(0.0f)
	,	MaxScreenSize(1.0f)
	,	DynamicIndexData(NULL)
	{
	}
};

/**
 * A batch of mesh elements, all with the same material and vertex buffer
 */
struct FMeshBatch
{
	TArray<FMeshBatchElement,TInlineAllocator<1> > Elements;

	// used with DynamicVertexData
	uint16 DynamicVertexStride;

	/** LOD index of the mesh, used for fading LOD transitions. */
	int8 LODIndex;

	uint32 UseDynamicData : 1;
	uint32 ReverseCulling : 1;
	uint32 bDisableBackfaceCulling : 1;
	uint32 CastShadow : 1;
	uint32 bWireframe : 1;
	// e.g. PT_TriangleList(default), PT_LineList, ..
	uint32 Type : PT_NumBits;
	// e.g. SDPG_World (default), SDPG_Foreground
	uint32 DepthPriorityGroup : SDPG_NumBits;
	uint32 bUseAsOccluder : 1;

	/** Whether view mode overrides can be applied to this mesh eg unlit, wireframe. */
	uint32 bCanApplyViewModeOverrides : 1;

	/** 
	 * Whether to treat the batch as selected in special viewmodes like wireframe. 
	 * This is needed instead of just Proxy->IsSelected() because some proxies do weird things with selection highlighting, like FStaticMeshSceneProxy.
	 */
	uint32 bUseWireframeSelectionColoring : 1;

	/** 
	 * Whether the batch should receive the selection outline.  
	 * This is useful for proxies which support selection on a per-mesh batch basis.
	 * They submit multiple mesh batches when selected, some of which have bUseSelectionOutline enabled.
	 */
	uint32 bUseSelectionOutline : 1;

	/** Whether the mesh batch can be selected through editor selection, aka hit proxies. */
	uint32 bSelectable : 1;

	// can be NULL
	const FLightCacheInterface* LCI;

	/** 
	 *	DynamicVertexData - pointer to user memory containing the vertex data.
	 *	Used for rendering dynamic data directly.
	 *  used with DynamicVertexStride
	 */
	const void* DynamicVertexData;

	/** Vertex factory for rendering, required. */
	const FVertexFactory* VertexFactory;

	/** Material proxy for rendering, required. */
	const FMaterialRenderProxy* MaterialRenderProxy;

	/** The current hit proxy ID being rendered. */
	FHitProxyId BatchHitProxyId;

	FORCEINLINE bool IsTranslucent(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		// Note: blend mode does not depend on the feature level we are actually rendering in.
		return IsTranslucentBlendMode(MaterialRenderProxy->GetMaterial(InFeatureLevel)->GetBlendMode());
	}

	FORCEINLINE bool IsMasked(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		// Note: blend mode does not depend on the feature level we are actually rendering in.
		return MaterialRenderProxy->GetMaterial(InFeatureLevel)->IsMasked();
	}

	/** Converts from an int32 index into a int8 */
	static int8 QuantizeLODIndex(int32 NewLODIndex)
	{
		checkSlow(NewLODIndex >= SCHAR_MIN && NewLODIndex <= SCHAR_MAX);
		return (int8)NewLODIndex;
	}

	/** 
	* @return vertex stride specified for the mesh. 0 if not dynamic
	*/
	FORCEINLINE uint32 GetDynamicVertexStride(ERHIFeatureLevel::Type /*InFeatureLevel*/) const
	{
		if (UseDynamicData && DynamicVertexData)
		{
			return DynamicVertexStride;
		}
		else
		{
			return 0;
		}
	}

	FORCEINLINE int32 GetNumPrimitives() const
	{
		int32 Count=0;
		for( int32 ElementIdx=0;ElementIdx<Elements.Num();ElementIdx++ )
		{
			Count += Elements[ElementIdx].NumPrimitives * Elements[ElementIdx].NumInstances;
		}
		return Count;
	}

#if DO_CHECK
	FORCEINLINE void CheckUniformBuffers() const
	{
		for( int32 ElementIdx=0;ElementIdx<Elements.Num();ElementIdx++ )
		{
			check(IsValidRef(Elements[ElementIdx].PrimitiveUniformBuffer) || Elements[ElementIdx].PrimitiveUniformBufferResource != NULL);
		}
	}
#endif	

	/** Default constructor. */
	FMeshBatch()
	:	DynamicVertexStride(0)
	,	LODIndex(INDEX_NONE)
	,	UseDynamicData(false)
	,	ReverseCulling(false)
	,	bDisableBackfaceCulling(false)
	,	CastShadow(true)
	,	bWireframe(false)
	,	Type(PT_TriangleList)
	,	DepthPriorityGroup(SDPG_World)
	,	bUseAsOccluder(true)
	,	bCanApplyViewModeOverrides(false)
	,	bUseWireframeSelectionColoring(false)
	,	bUseSelectionOutline(true)
	,	bSelectable(true)
	,	LCI(NULL)
	,	DynamicVertexData(NULL)
	,	VertexFactory(NULL)
	,	MaterialRenderProxy(NULL)
	{
		// By default always add the first element.
		new(Elements) FMeshBatchElement;
	}
};