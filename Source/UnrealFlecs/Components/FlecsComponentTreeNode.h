// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsComponentTreeNode.generated.h"

class UFlecsComponentTreeObject;

UENUM(BlueprintType)
enum class EFlecsComponentTreeNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
}; // enum class EFlecsComponentTreeNodeType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsComponentTypeInfo final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsComponentTreeNodeType NodeType = EFlecsComponentTreeNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentTreeNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentTreeNodeType::EntityHandle", EditConditionHides))
	FFlecsEntityHandle EntityHandle;
	
}; // struct FFlecsComponentTypeInfo

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsComponentTreeNode final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (ShowOnlyInnerProperties))
	FFlecsComponentTypeInfo TypeInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	TArray<TObjectPtr<UFlecsComponentTreeObject>> Children;
	
}; // struct FFlecsComponentTreeNode

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, meta = (DisplayName = "Flecs Component Tree Object"))
class UNREALFLECS_API UFlecsComponentTreeObject final : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsComponentTreeNode TreeNode;
}; // class UFlecsComponentTreeObject