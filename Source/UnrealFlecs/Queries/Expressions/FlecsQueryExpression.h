// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs/addons/cpp/mixins/query/builder.hpp"
#include "FlecsQueryExpression.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryExpression
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsQueryExpression() = default;
	virtual ~FFlecsQueryExpression() = default;
	
	virtual void Apply(flecs::query_builder<>& InQueryBuilder) PURE_VIRTUAL(FFlecsQueryExpression::Apply, );
	
}; // struct FFlecsQueryExpression

template<>
struct TStructOpsTypeTraits<FFlecsQueryExpression> : public TStructOpsTypeTraitsBase2<FFlecsQueryExpression>
{
	enum
	{
		WithPureVirtual = true
	};
}; // struct TStructOpsTypeTraits<FFlecsQueryExpression>
