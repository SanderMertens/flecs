// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQueryOptionalExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryOptionalExpression)

FFlecsQueryOptionalExpression::FFlecsQueryOptionalExpression() : Super(false)
{
}

void FFlecsQueryOptionalExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.optional();
}
