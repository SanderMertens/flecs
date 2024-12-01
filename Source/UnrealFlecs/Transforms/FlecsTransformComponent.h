// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Standard/Hashing.h"
#include "FlecsTransformComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTransformComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsTransformComponent& InComponent)
	{
		return GetTypeHash(InComponent.Transform);
	}

	FORCEINLINE FFlecsTransformComponent() = default;

	FORCEINLINE FFlecsTransformComponent(const FTransform& InTransform)
		: Transform(InTransform)
	{
	}

	FORCEINLINE NO_DISCARD FTransform& GetTransform() { return Transform; }
	FORCEINLINE NO_DISCARD const FTransform& GetTransform() const { return Transform; }
	
	FORCEINLINE operator FTransform&() { return GetTransform(); }
	FORCEINLINE operator const FTransform&() const { return GetTransform(); }

	FORCEINLINE FFlecsTransformComponent& operator=(const FTransform& InTransform)
	{
		Transform = InTransform;
		return *this;
	}

	FORCEINLINE FFlecsTransformComponent& operator=(const FFlecsTransformComponent& InComponent)
	{
		Transform = InComponent.Transform;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsTransformComponent& InComponent) const
	{
		return Transform.Equals(InComponent.Transform);
	}

	FORCEINLINE bool operator!=(const FFlecsTransformComponent& InComponent) const
    {
		return !(*this == InComponent);
    }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FTransform Transform;

}; // struct FFlecsTransformComponent

DEFINE_STD_HASH(FFlecsTransformComponent);