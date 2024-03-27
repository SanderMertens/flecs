// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "FlecsNetworkIdComponent.h"
#include "Components/ActorComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "FlecsNetworkingActorComponent.generated.h"

USTRUCT()
struct FNetworkedEntityInfo
{
	UPROPERTY()
	FFlecsNetworkIdComponent NetworkId;

	UPROPERTY()
	FName WorldName = FName("DefaultFlecsWorld");

	UPROPERTY()
	FName EntityName;
}; // struct FNetworkedEntityInfo

UCLASS(BlueprintType, ClassGroup=(Flecs), meta=(BlueprintSpawnableComponent))
class UNREALFLECS_API UFlecsNetworkingActorComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlecsNetworkingActorComponent(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		SetIsReplicatedByDefault(true);
	}
	
	FORCEINLINE virtual void BeginPlay() override
	{
		Super::BeginPlay();

		checkf(GetOwner()->IsA<APlayerController>(),
			TEXT("Owner of UFlecsNetworkingActorComponent must be a APlayerController"));

		if (GetOwner()->HasAuthority())
		{
			
		}
		else
		{
			
		}
	}

	FORCEINLINE virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
		FDoRepLifetimeParams Params;
		Params.bIsPushBased = true;

		DOREPLIFETIME_WITH_PARAMS_FAST(UFlecsNetworkingActorComponent, NetworkOwnedEntities, Params);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE void AddNetworkOwnedEntity(const FFlecsNetworkIdComponent& Entity)
	{
		checkf(Entity.IsValid(), TEXT("Entity must be valid"));
		NetworkOwnedEntities.Add(Entity);
		SortNetworkOwnedEntities();

		MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsNetworkingActorComponent, NetworkOwnedEntities, this);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE void RemoveNetworkOwnedEntity(const FFlecsNetworkIdComponent& Entity)
	{
		NetworkOwnedEntities.Remove(Entity);
		SortNetworkOwnedEntities();

		MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsNetworkingActorComponent, NetworkOwnedEntities, this);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE bool HasNetworkOwnedEntity(const FFlecsNetworkIdComponent& Entity) const
	{
		return GetNetworkOwnedEntities().Contains(Entity);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE void ClearNetworkOwnedEntities()
	{
		NetworkOwnedEntities.Empty();
		
		MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsNetworkingActorComponent, NetworkOwnedEntities, this);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE TSet<FFlecsNetworkIdComponent> GetNetworkOwnedEntities() const
	{
		return static_cast<TSet<FFlecsNetworkIdComponent>>(NetworkOwnedEntities);
	}

private:
	UPROPERTY(Replicated)
	TArray<FFlecsNetworkIdComponent> NetworkOwnedEntities;

	UFUNCTION(Client, Reliable)
	void Client_UpdateNetworkedEntities(const TArray<FNetworkedEntityInfo>& Entities);

	FORCEINLINE void SortNetworkOwnedEntities()
	{
		NetworkOwnedEntities.Sort([](const FFlecsNetworkIdComponent& A, const FFlecsNetworkIdComponent& B)
		{
			return A.GetNetworkId() < B.GetNetworkId();
		});
	}

}; // class UFlecsNetworkingActorComponent
