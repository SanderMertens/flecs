// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQueryTraitExpression.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryTraitExpression)

FFlecsQueryTraitExpression::FFlecsQueryTraitExpression() : Super(true)
{
}

void FFlecsQueryTraitExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	switch (ComponentInputType)
	{
		case EFlecsQueryTraitInputType::LastTerm:
			{
				InQueryBuilder.begin_scope_traits();
			}
			break;
		case EFlecsQueryTraitInputType::ScriptStruct:
			{
				solid_checkf(IsValid(ComponentStruct),
					TEXT("TraitStruct must be valid when TraitType is ScriptStruct"));
				InQueryBuilder.begin_scope_traits(
					InWorld->ObtainComponentTypeStruct(ComponentStruct));
			}
			break;
		case EFlecsQueryTraitInputType::String:
			{
				solid_checkf(!ComponentName.IsEmpty(),
					TEXT("TraitName must not be empty when TraitType is String"));
				InQueryBuilder.begin_scope_traits(StringCast<char>(*ComponentName).Get());
			}
			break;
	};

	if (bIsExclusionary)
	{
		InQueryBuilder.optional();
	}
	
	Super::Apply(InWorld, InQueryBuilder);

	InQueryBuilder.end_scope_traits();
}
