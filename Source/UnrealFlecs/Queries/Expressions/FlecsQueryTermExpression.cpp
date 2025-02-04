// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryTermExpression.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryTermExpression)

FFlecsQueryTermExpression::FFlecsQueryTermExpression() : Super(true)
{
}

void FFlecsQueryTermExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	switch (InputType.Type)
	{
		case EFlecsQueryInputType::ScriptStruct:
			{
				const UScriptStruct* Struct = InputType.ScriptStruct;
				solid_checkf(IsValid(Struct), TEXT("Invalid ScriptStruct provided for query term expression"));
				
				const FFlecsEntityHandle ScriptStructEntity = InWorld->ObtainComponentTypeStruct(Struct);
				solid_check(ScriptStructEntity.IsValid());
				
				InQueryBuilder.with(ScriptStructEntity.GetEntity());
			}
		break;
		case EFlecsQueryInputType::Entity:
			{
				const FFlecsEntityHandle Entity = InputType.Entity;
				InQueryBuilder.with(Entity.GetEntity());
			}
		break;
		case EFlecsQueryInputType::String:
			{
				const FString Expr = InputType.Expr.Expr;
				InQueryBuilder.with(StringCast<char>(*Expr).Get());
			}
		break;
		case EFlecsQueryInputType::GameplayTag:
			{
				const FGameplayTag Tag = InputType.Tag;
				solid_checkf(Tag.IsValid(), TEXT("Invalid GameplayTag provided for query term expression"));
				
				const FFlecsEntityHandle TagEntity = InWorld->GetTagEntity(Tag);
				solid_check(TagEntity.IsValid());
				
				InQueryBuilder.with(TagEntity.GetEntity());
			}
		break;
	}

	if (bWithout)
	{
		InQueryBuilder.not_();
	}

	Super::Apply(InWorld, InQueryBuilder);
}