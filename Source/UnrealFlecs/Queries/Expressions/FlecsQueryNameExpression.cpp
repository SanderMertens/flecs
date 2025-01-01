// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQueryNameExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryNameExpression)

FFlecsQueryNameExpression::FFlecsQueryNameExpression() : Super(false)
{
}

void FFlecsQueryNameExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	solid_check(!Name.IsEmpty());
	InQueryBuilder.name(StringCast<char>(*Name).Get());
}
