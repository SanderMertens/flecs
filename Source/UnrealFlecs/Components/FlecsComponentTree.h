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
	
}; // struct FFlecsComponentTree
