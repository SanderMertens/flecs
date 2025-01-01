// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsQueryScriptExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryScriptExpression)

FFlecsQueryScriptExpression::FFlecsQueryScriptExpression() : Super(true)
{
}

void FFlecsQueryScriptExpression::Apply(UFlecsWorld* InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.expr(StringCast<char>(*ScriptExpr.Expr).Get());
	
	Super::Apply(InWorld, InQueryBuilder);
}
