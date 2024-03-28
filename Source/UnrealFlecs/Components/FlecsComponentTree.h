// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsComponentTreeNode.h"
#include "FlecsComponentTree.generated.h"

USTRUCT(BlueprintType)
struct FFlecsComponentTree
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	TArray<FFlecsComponentTreeNode> ComponentTree;

	FORCEINLINE void ApplyTreeToEntity(const FFlecsEntityHandle& EntityHandle) const
	{
		for (const FFlecsComponentTreeNode& Node : ComponentTree)
		{
			
		}
	}

private:

	FORCEINLINE void ApplyTreeNodeToEntity(const FFlecsComponentTreeNode& Node, const FFlecsEntityHandle& EntityHandle) const
	{
		switch (Node.TypeInfo.NodeType)
		{
		case EFlecsComponentTreeNodeType::ScriptStruct:
			break;
		case EFlecsComponentTreeNodeType::EntityHandle:
			break;
		case EFlecsComponentTreeNodeType::FGameplayTag:
			break;
		}
	}

	FORCEINLINE void ApplyTreeObjectToEntity(const UFlecsComponentTreeObject* TreeObject, const FFlecsEntityHandle& EntityHandle) const
	{
		
	}
	
}; // struct FFlecsComponentTree
