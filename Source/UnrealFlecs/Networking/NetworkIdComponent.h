// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "NetworkIdComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FNetworkIdComponent
{
	GENERATED_BODY()

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FNetworkIdComponent& InComponent)
	{
		return GetTypeHash(InComponent.GetNetworkId());
	}

public:
	FORCEINLINE FNetworkIdComponent() = default;
	FORCEINLINE FNetworkIdComponent(const uint64 InNetworkId) : NetworkId(InNetworkId) {}

	FORCEINLINE NO_DISCARD uint64 GetNetworkId() const { return NetworkId; }
	FORCEINLINE void SetNetworkId(const uint64 InNetworkId) { NetworkId = InNetworkId; }

	FORCEINLINE bool operator==(const FNetworkIdComponent& Other) const
	{
		return NetworkId == Other.NetworkId;
	}

	FORCEINLINE bool operator!=(const FNetworkIdComponent& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE bool operator==(const uint64 Other) const
	{
		return NetworkId == Other;
	}

	FORCEINLINE bool operator!=(const uint64 Other) const
	{
		return !(*this == Other);
	}

private:
	UPROPERTY()
	uint64 NetworkId = std::numeric_limits<uint64>::max();
	
}; // struct FNetworkIdComponent
