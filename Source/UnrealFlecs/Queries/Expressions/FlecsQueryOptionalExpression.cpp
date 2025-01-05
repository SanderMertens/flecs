// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsQueryOptionalExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryOptionalExpression)

FFlecsQueryOptionalExpression::FFlecsQueryOptionalExpression() : Super(false)
{
}

void FFlecsQueryOptionalExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.optional();
}
