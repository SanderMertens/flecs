// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQueryFlags.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class EFlecsQueryFlags : uint8
{
	None = 0,
	MatchPrefabs = EcsQueryMatchPrefab,
	MatchDisabled = EcsQueryMatchDisabled,
	MatchEmptyTables = EcsQueryMatchEmptyTables,
	AllowUnresolvedByName = EcsQueryAllowUnresolvedByName,
	TableOnly = EcsQueryTableOnly,
}; // enum class EFlecsQueryFlags

ENUM_CLASS_FLAGS(EFlecsQueryFlags)

