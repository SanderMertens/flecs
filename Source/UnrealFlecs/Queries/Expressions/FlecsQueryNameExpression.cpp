// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryNameExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryNameExpression)

FFlecsQueryNameExpression::FFlecsQueryNameExpression() : Super(false)
{
}

void FFlecsQueryNameExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	solid_checkf(!Name.IsEmpty(), TEXT("Name is empty"));
	InQueryBuilder.name(StringCast<char>(*Name).Get());
}
