// Elie Wiese-Namir © 2024. All Rights Reserved.

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

