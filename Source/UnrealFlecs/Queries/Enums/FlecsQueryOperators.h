// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsQueryOperators.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryOperator : uint8
{
    /**< The term must match */
    And = flecs::And,
    /**< One of the terms in an or chain must match */
    Or = flecs::Or,
    /**< The term must not match */
    Not = flecs::Not,
    /**< The term may match */
    Optional = flecs::Optional,
    /**< Term must match all components from term id */
    AndFrom = flecs::AndFrom,
    /**< Term must match at least one component from term id */
    OrFrom = flecs::OrFrom,
    /**< Term must match none of the components from term id */
    NotFrom = flecs::NotFrom,
    // Defaults to And
    Default = flecs::And
}; // enum class EFlecsQueryOperator

NO_DISCARD FORCEINLINE constexpr flecs::oper_kind_t ToFlecsOperator(EFlecsQueryOperator Operator)
{
    return static_cast<flecs::oper_kind_t>(Operator);
} 
