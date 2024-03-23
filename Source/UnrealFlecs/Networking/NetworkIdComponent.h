// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "NetworkIdComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FNetworkIdComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FNetworkIdComponent() = default;
	FORCEINLINE FNetworkIdComponent(const uint64 InNetworkId) : NetworkId(InNetworkId) {}

	FORCEINLINE NO_DISCARD uint64 GetNetworkId() const { return NetworkId; }
	FORCEINLINE void SetNetworkId(const uint64 InNetworkId) { NetworkId = InNetworkId; }

private:
	UPROPERTY()
	uint64 NetworkId;
	
}; // struct FNetworkIdComponent
