// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsScaleComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsScaleComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsScaleComponent& InComponent)
	{
		return GetTypeHash(InComponent.Scale);
	}

	FORCEINLINE FFlecsScaleComponent() = default;

	FORCEINLINE FFlecsScaleComponent(const FVector& InScale)
		: Scale(InScale)
	{
	}

	FORCEINLINE NO_DISCARD FVector GetScale() const { return Scale; }

	FORCEINLINE FFlecsScaleComponent& operator=(const FVector& InScale)
	{
		Scale = InScale;
		return *this;
	}

	FORCEINLINE FFlecsScaleComponent& operator=(const FFlecsScaleComponent& InComponent)
	{
		Scale = InComponent.Scale;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsScaleComponent& InComponent) const
	{
		return Scale == InComponent.Scale;
	}

	FORCEINLINE bool operator!=(const FFlecsScaleComponent& InComponent) const
	{
		return !(*this == InComponent);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FVector Scale;

}; // struct FFlecsScaleComponent
