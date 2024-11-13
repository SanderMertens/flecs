// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsDependenciesComponent.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsDependenciesComponent
{
	GENERATED_BODY()

public:
	TMap<UClass*, std::function<void(UObject*, UFlecsWorld*, FFlecsEntityHandle)>> Dependencies;
}; // struct FFlecsDependenciesComponent
