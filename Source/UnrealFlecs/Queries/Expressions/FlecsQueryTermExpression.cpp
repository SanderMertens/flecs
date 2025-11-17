// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryTermExpression.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryTermExpression)

FFlecsQueryTermExpression::FFlecsQueryTermExpression() : Super(true /* bInAllowsChildExpressions */)
{
}

void FFlecsQueryTermExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	switch (InputType.Type)
	{
		case EFlecsQueryInputType::ScriptStruct:
			{
				const UScriptStruct* Struct = InputType.ScriptStruct;
				solid_checkf(IsValid(Struct),
					TEXT("Invalid ScriptStruct provided for query term expression"));
				
				const FFlecsEntityHandle ScriptStructEntity = InWorld->GetScriptStructEntity(Struct);
				solid_check(ScriptStructEntity.IsValid());
				
				InQueryBuilder.with(ScriptStructEntity);

				break;
			}
		case EFlecsQueryInputType::Entity:
			{
				const FFlecsId Entity = InputType.Entity;
				InQueryBuilder.with(Entity);

				break;
			}
		case EFlecsQueryInputType::String:
			{
				const FString Expr = InputType.Expr.Expr;
				InQueryBuilder.with(StringCast<char>(*Expr).Get());
				
				break;
			}
		case EFlecsQueryInputType::GameplayTag:
			{
				const FGameplayTag Tag = InputType.Tag;
				solid_checkf(Tag.IsValid(),
					TEXT("Invalid GameplayTag provided for query term expression"));
				
				const FFlecsEntityHandle TagEntity = InWorld->GetTagEntity(Tag);
				solid_check(TagEntity.IsValid());
				
				InQueryBuilder.with(TagEntity);

				break;
			}
	}

	if (bWithout)
	{
		InQueryBuilder.not_();
	}

	Super::Apply(InWorld, InQueryBuilder);
}