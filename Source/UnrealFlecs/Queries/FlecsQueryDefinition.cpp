// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryDefinition)

void FFlecsQueryDefinition::Apply(const TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.cache_kind(static_cast<flecs::query_cache_kind_t>(CacheType));
	InQueryBuilder.query_flags(Flags);

	if (bDetectChanges)
	{
		InQueryBuilder.detect_changes();
	}
		
	for (const FFlecsQueryTermExpression& Term : Terms)
	{
		Term.Apply(InWorld, InQueryBuilder);
	}

	for (const TInstancedStruct<FFlecsQueryExpression>& OtherExpression : OtherExpressions)
	{
		OtherExpression.Get<FFlecsQueryExpression>().Apply(InWorld, InQueryBuilder);
	}
}
