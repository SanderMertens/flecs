// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsOperQueryExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsOperQueryExpression)

FFlecsOperQueryExpression::FFlecsOperQueryExpression() : Super(false)
{
}

void FFlecsOperQueryExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.oper(static_cast<flecs::oper_kind_t>(Operator));
}
