// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQuerySrcExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQuerySrcExpression)

FFlecsQuerySrcExpression::FFlecsQuerySrcExpression() : Super(false)
{
}

void FFlecsQuerySrcExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	switch (SrcType)
	{
		case EFlecsQuerySrcType::Entity:
			{
				InQueryBuilder.src(Entity.GetEntity());
			}
			break;
		case EFlecsQuerySrcType::String:
			{
				InQueryBuilder.src(StringCast<char>(*Src).Get());
			}
			break;
		default: ;
	}
}
