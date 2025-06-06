// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryFilterExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryFilterExpression)

FFlecsQueryFilterExpression::FFlecsQueryFilterExpression() : Super(false /* bInAllowsChildExpressions */)
{
}

void FFlecsQueryFilterExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.filter();
}
