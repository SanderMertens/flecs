// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQuerySingletonExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQuerySingletonExpression)

FFlecsQuerySingletonExpression::FFlecsQuerySingletonExpression() : Super(false)
{
}

void FFlecsQuerySingletonExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.singleton();
}
