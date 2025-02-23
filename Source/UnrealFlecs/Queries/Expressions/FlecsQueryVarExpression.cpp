// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryVarExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryVarExpression)

FFlecsQueryVarExpression::FFlecsQueryVarExpression() : Super(false /* bInAllowsChildExpressions */)
{
}

void FFlecsQueryVarExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.var(StringCast<char>(*VarName).Get());
}
