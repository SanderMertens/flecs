// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsTickerComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTickerComponent final
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsTickerComponent() = default;
	FORCEINLINE FFlecsTickerComponent(const uint64 InTickId) : TickId(InTickId) {}

	UPROPERTY()
	uint64 TickId = 0;
	
}; // struct FFlecsTickerComponent

REGISTER_FLECS_COMPONENT_PROPERTIES(FFlecsTickerComponent, { flecs::Sparse }, { })

