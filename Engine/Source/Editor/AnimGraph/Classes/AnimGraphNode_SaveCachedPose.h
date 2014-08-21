// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode_SaveCachedPose.h"
#include "AnimGraphNode_SaveCachedPose.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_SaveCachedPose : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_SaveCachedPose Node;

	UPROPERTY(EditAnywhere, Category=CachedPose)
	FString CacheName;

	// UEdGraphNode interface
	virtual FString GetTooltip() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	// End of UEdGraphNode interface

	// UK2Node interface.
	virtual void GetMenuActions(TArray<UBlueprintNodeSpawner*>& ActionListOut) const override;
	// End of UK2Node interface

	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	virtual void GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual bool IsSinkNode() const override { return true; }
	// End of UAnimGraphNode_Base interface
};
