// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsQuerySrcExpression.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsQuerySrcExpression)

FFlecsQuerySrcExpression::FFlecsQuerySrcExpression() : Super(false /* bInAllowsChildExpressions */)
{
}

void FFlecsQuerySrcExpression::Apply(TSolidNotNull<UFlecsWorld*> InWorld, flecs::query_builder<>& InQueryBuilder) const
{
	switch (SrcType)
	{
		case EFlecsQuerySrcType::Entity:
			{
				InQueryBuilder.src(Entity);

				break;
			}
		case EFlecsQuerySrcType::String:
			{
				InQueryBuilder.src(StringCast<char>(*Src).Get());

				break;
			}
	}
}
