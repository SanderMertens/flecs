// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Macros.h"
#include "FlecsQueryOperators.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryOperator : uint8
{
    And = EcsAnd,
    Or = EcsOr,
    Not = EcsNot,
    Optional = EcsOptional,
    AndFrom = EcsAndFrom,
    OrFrom = EcsOrFrom,
    NotFrom = EcsNotFrom,
}; // enum class EFlecsQueryOperator

FORCEINLINE NO_DISCARD constexpr flecs::oper_kind_t ToFlecsOperator(EFlecsQueryOperator Operator) NOEXCEPT
{
    return static_cast<flecs::oper_kind_t>(Operator);
} 
