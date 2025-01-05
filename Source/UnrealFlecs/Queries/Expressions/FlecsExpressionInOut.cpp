// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsExpressionInOut.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsExpressionInOut)

FFlecsExpressionInOut::FFlecsExpressionInOut() : Super(false)
{
}

void FFlecsExpressionInOut::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	if (bStage)
	{
		InQueryBuilder.inout_stage(static_cast<flecs::inout_kind_t>(InOut));
	}
	else
	{
		InQueryBuilder.inout(static_cast<flecs::inout_kind_t>(InOut));
	}
}
