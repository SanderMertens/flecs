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
			ApplyTreeNodeToEntity(Node, EntityHandle);
		}
	}

private:

	FORCEINLINE void ApplyTreeNodeToEntity(const FFlecsComponentTreeNode& Node, const FFlecsEntityHandle& EntityHandle) const
	{
		switch (Node.TypeInfo.NodeType)
		{
		case EFlecsComponentTreeNodeType::ScriptStruct:
			EntityHandle.Set(Node.TypeInfo.ScriptStruct);
			break;
		case EFlecsComponentTreeNodeType::EntityHandle:
			EntityHandle.Add(Node.TypeInfo.EntityHandle);
			break;
		case EFlecsComponentTreeNodeType::FGameplayTag:
			EntityHandle.Add(Node.TypeInfo.GameplayTag);
			break;
		}

		if (Node.Children.Num() > 0)
		{
			for (const UFlecsComponentTreeObject* Child : Node.Children)
			{
				ApplyTreeNodeToEntity(Child->TreeNode, EntityHandle);
			}
		}
	}
	
}; // struct FFlecsComponentTree
