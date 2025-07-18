// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQuerySingletonExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQuerySingletonExpression)

FFlecsQuerySingletonExpression::FFlecsQuerySingletonExpression() : Super(false /* bInAllowsChildExpressions */)
{
}

void FFlecsQuerySingletonExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	//InQueryBuilder.singleton();
}
