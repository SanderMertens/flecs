// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsNetworkIdComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsNetworkIdComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsNetworkIdComponent& InComponent)
	{
		return GetTypeHash(InComponent.GetNetworkId());
	}

public:
	FORCEINLINE FFlecsNetworkIdComponent() = default;
	FORCEINLINE FFlecsNetworkIdComponent(const uint32 InNetworkId) : NetworkId(InNetworkId) {}

	FORCEINLINE NO_DISCARD uint32 GetNetworkId() const { return NetworkId.Get(INDEX_NONE); }
	FORCEINLINE void SetNetworkId(const uint32 InNetworkId) { NetworkId = InNetworkId; }

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

	FORCEINLINE bool operator==(const uint32 Other) const
	{
		return NetworkId == Other;
	}

	FORCEINLINE bool operator!=(const uint32 Other) const
	{
		return !(*this == Other);
	}
	
	UPROPERTY()
	TOptional<uint32> NetworkId;

	FORCEINLINE NO_DISCARD FString ToString() const
	{
		return FString::Printf(TEXT("NetworkId: %lu"), NetworkId.Get(INDEX_NONE));
	}

	FORCEINLINE bool NetSerialize(FArchive& Ar, MAYBE_UNUSED UPackageMap* Map, bool& bOutSuccess)
	{
		SerializeOptionalValue<TOptional<uint32>>(Ar.IsSaving(), Ar, NetworkId, TOptional<uint32>());

		bOutSuccess = true;
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
