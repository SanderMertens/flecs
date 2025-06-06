// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQueryScriptExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQueryScriptExpression)

FFlecsQueryScriptExpression::FFlecsQueryScriptExpression() : Super(true /* bInAllowsChildExpressions */)
{
}

void FFlecsQueryScriptExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	InQueryBuilder.expr(StringCast<char>(*ScriptExpr.Expr).Get());
	
	Super::Apply(InWorld, InQueryBuilder);
}
