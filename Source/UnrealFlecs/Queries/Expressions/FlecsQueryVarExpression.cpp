// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsQueryVarExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryVarExpression)

FFlecsQueryVarExpression::FFlecsQueryVarExpression() : Super(false)
{
}

void FFlecsQueryVarExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.var(StringCast<char>(*VarName).Get());
}
