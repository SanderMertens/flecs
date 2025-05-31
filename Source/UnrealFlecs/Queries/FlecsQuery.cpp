// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQuery.h"
#include "Components/FlecsWorldPtrComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQuery)

FFlecsQuery::FFlecsQuery(const TSolidNonNullPtr<UFlecsWorld> InFlecsWorld, const FString& InName, const FFlecsQueryDefinition& InDefinition)
{
	flecs::query_builder<> QueryBuilder = InFlecsWorld->World.query_builder<>(StringCast<char>(*InName).Get());
	InDefinition.Apply(InFlecsWorld, QueryBuilder);
	Query = QueryBuilder.build();
}
