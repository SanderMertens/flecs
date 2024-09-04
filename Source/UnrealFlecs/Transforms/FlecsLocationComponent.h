// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsLocationComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsLocationComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsLocationComponent& InComponent)
	{
		return GetTypeHash(InComponent.Location);
	}

	FORCEINLINE FFlecsLocationComponent() = default;

	FORCEINLINE FFlecsLocationComponent(const FVector& InLocation)
		: Location(InLocation)
	{
	}

	FORCEINLINE NO_DISCARD FVector GetLocation() const { return Location; }

	FORCEINLINE FFlecsLocationComponent& operator=(const FVector& InLocation)
	{
		Location = InLocation;
		return *this;
	}

	FORCEINLINE FFlecsLocationComponent& operator=(const FFlecsLocationComponent& InComponent)
	{
		Location = InComponent.Location;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsLocationComponent& InComponent) const
	{
		return Location == InComponent.Location;
	}
	
	FORCEINLINE bool operator!=(const FFlecsLocationComponent& InComponent) const
	{
		return !(*this == InComponent);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FVector Location;

}; // struct FFlecsLocationComponent
