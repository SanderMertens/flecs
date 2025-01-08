// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryExpression)

void FFlecsQueryExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	for (const TInstancedStruct<FFlecsQueryExpression>& Child : Children)
	{
		Child.GetPtr<FFlecsQueryExpression>()->Apply(InWorld, InQueryBuilder);
	}
}
