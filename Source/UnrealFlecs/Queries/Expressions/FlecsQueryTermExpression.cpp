// Fill out your copyright notice in the Description page of Project Settings.

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
				const UScriptStruct* Struct = InputType.ScriptStruct.Get();
				const FFlecsEntityHandle ScriptStructEntity = InWorld->ObtainComponentTypeStruct(Struct);
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
				const FFlecsEntityHandle TagEntity = InWorld->GetTagEntity(Tag);
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