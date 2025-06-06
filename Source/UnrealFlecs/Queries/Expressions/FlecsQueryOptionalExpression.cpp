// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryOptionalExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryOptionalExpression)

FFlecsQueryOptionalExpression::FFlecsQueryOptionalExpression() : Super(false /* bInAllowsChildExpressions */)
{
}

void FFlecsQueryOptionalExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.optional();
}
