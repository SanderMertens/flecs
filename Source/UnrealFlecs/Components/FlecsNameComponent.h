// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsNameComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsNameComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsNameComponent() = default;
	FORCEINLINE FFlecsNameComponent(const FString& InName) : Name(InName) {}

	FORCEINLINE void SetName(const FString& InName) { Name = InName; }
	FORCEINLINE NO_DISCARD FString GetName() const { return Name; }

	FORCEINLINE operator FString() const { return GetName(); }

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsNameComponent& Other) const
	{
		return Name == Other.Name;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsNameComponent& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Components")
	FString Name;
	
}; // struct FFlecsNameComponent
