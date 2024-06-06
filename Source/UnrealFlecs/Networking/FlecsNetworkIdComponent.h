// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsNetworkIdComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsNetworkIdComponent
{
	GENERATED_BODY()

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsNetworkIdComponent& InComponent)
	{
		return GetTypeHash(InComponent.GetNetworkId());
	}

public:
	FORCEINLINE FFlecsNetworkIdComponent() = default;
	FORCEINLINE FFlecsNetworkIdComponent(const uint64 InNetworkId) : NetworkId(InNetworkId) {}

	FORCEINLINE NO_DISCARD uint64 GetNetworkId() const { return NetworkId.Get(std::numeric_limits<uint64>::max()); }
	FORCEINLINE void SetNetworkId(const uint64 InNetworkId) { NetworkId = InNetworkId; }

	FORCEINLINE NO_DISCARD bool IsValid() const { return NetworkId.IsSet(); }

	FORCEINLINE void Reset() { NetworkId.Reset(); }

	FORCEINLINE bool operator==(const FFlecsNetworkIdComponent& Other) const
	{
		return NetworkId == Other.NetworkId;
	}

	FORCEINLINE bool operator!=(const FFlecsNetworkIdComponent& Other) const
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
	
	UPROPERTY()
	TOptional<uint64> NetworkId;

	FORCEINLINE NO_DISCARD FString ToString() const
	{
		return FString::Printf(TEXT("NetworkId: %llu"), NetworkId.Get(std::numeric_limits<uint64>::max()));
	}

	FORCEINLINE bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		const TOptional InvalidNetworkId = std::numeric_limits<uint64>::max();
		
		SerializeOptionalValue<TOptional<uint64>>(Ar.IsSaving(), Ar, NetworkId, InvalidNetworkId);
		
		return true;
	}
	
}; // struct FNetworkIdComponent

template<>
struct TStructOpsTypeTraits<FFlecsNetworkIdComponent> : public TStructOpsTypeTraitsBase2<FFlecsNetworkIdComponent>
{
	enum
	{
		WithNetSerializer = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsNetworkIdComponent>
