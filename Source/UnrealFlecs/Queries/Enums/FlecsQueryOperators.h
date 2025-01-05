// Elie Wiese-Namir © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsQueryOperators.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryOperator : uint8
{
    And = flecs::And,
    Or = flecs::Or,
    Not = flecs::Not,
    Optional = flecs::Optional,
    AndFrom = flecs::AndFrom,
    OrFrom = flecs::OrFrom,
    NotFrom = flecs::NotFrom,
    Default = flecs::And
}; // enum class EFlecsQueryOperator

FORCEINLINE NO_DISCARD constexpr flecs::oper_kind_t ToFlecsOperator(EFlecsQueryOperator Operator) NOEXCEPT
{
    return static_cast<flecs::oper_kind_t>(Operator);
} 
