// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "FlecsNetworkingActorComponent.generated.h"

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
	}

	FORCEINLINE virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
		FDoRepLifetimeParams Params;
		Params.bIsPushBased = true;

		DOREPLIFETIME_WITH_PARAMS_FAST(UFlecsNetworkingActorComponent, NetworkOwnedEntities, Params);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE void AddNetworkOwnedEntity(const FFlecsEntityHandle& Entity)
	{
		checkf(Entity.IsValid(), TEXT("Entity must be valid"));
		NetworkOwnedEntities.Add(Entity);
		SortNetworkOwnedEntities();

		MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsNetworkingActorComponent, NetworkOwnedEntities, this);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE void RemoveNetworkOwnedEntity(const FFlecsEntityHandle& Entity)
	{
		NetworkOwnedEntities.Remove(Entity);
		SortNetworkOwnedEntities();

		MARK_PROPERTY_DIRTY_FROM_NAME(UFlecsNetworkingActorComponent, NetworkOwnedEntities, this);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Networking")
	FORCEINLINE bool HasNetworkOwnedEntity(const FFlecsEntityHandle& Entity) const
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
	FORCEINLINE TSet<FFlecsEntityHandle> GetNetworkOwnedEntities() const
	{
		return static_cast<TSet<FFlecsEntityHandle>>(NetworkOwnedEntities);
	}

private:
	UPROPERTY(Replicated)
	TArray<FFlecsEntityHandle> NetworkOwnedEntities;

	FORCEINLINE void SortNetworkOwnedEntities()
	{
		NetworkOwnedEntities.Sort([](const FFlecsEntityHandle& A, const FFlecsEntityHandle& B)
		{
			return A.GetId() < B.GetId();
		});
	}

}; // class UFlecsNetworkingActorComponent
