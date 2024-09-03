// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsRotationComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRotationComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(FFlecsRotationComponent& InComponent)
	{
		return InComponent.Rotation.To
	}

	FORCEINLINE FFlecsRotationComponent() = default;

	FORCEINLINE FFlecsRotationComponent(const FRotator& InRotation)
		: Rotation(InRotation)
	{
	}

	FORCEINLINE NO_DISCARD FRotator GetRotation() const { return Rotation; }

	FORCEINLINE FFlecsRotationComponent& operator=(const FRotator& InRotation)
	{
		Rotation = InRotation;
		return *this;
	}

	FORCEINLINE FFlecsRotationComponent& operator=(const FFlecsRotationComponent& InComponent)
	{
		Rotation = InComponent.Rotation;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsRotationComponent& InComponent) const
	{
		return Rotation == InComponent.Rotation;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FRotator Rotation;

}; // struct FFlecsRotationComponent

