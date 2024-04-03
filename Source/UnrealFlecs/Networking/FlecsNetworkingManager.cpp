// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsNetworkingManager.h"
#include "FlecsEntitySyncInfoComponent.h"
#include "FlecsNetworkIdComponent.h"
#include "FlecsNetworkingActorComponent.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsNetworkingManager)

UNLOG_CATEGORY(LogFlecsNetworkingManager);

UFlecsNetworkingManager::UFlecsNetworkingManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UFlecsNetworkingManager::BeginPlay()
{
	Super::BeginPlay();

	FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(this);

	#if WITH_SERVER_CODE

	/* Networked Entities require a Stable Name to begin with,
	 this name can be changed after the entity's network ID is assigned */

	if (GetOwner()->HasAuthority())
	{
		NetworkIdObserver =
			FlecsWorld->CreateObserver<FFlecsNetworkIdComponent>(TEXT("NetworkingIdObserver"))
		.event(flecs::OnAdd)
		.yield_existing(true)
		.term<FFlecsEntitySyncInfoComponent>()
			.optional()
		.term(flecs::Name)
			.and_()
			.inout_none()
		.each([this](const FFlecsEntityHandle& Entity, FFlecsNetworkIdComponent& NetworkId,
			FFlecsEntitySyncInfoComponent* EntitySyncInfo)
		{
			if UNLIKELY_IF(NetworkId.IsValid())
			{
				return;
			}
			
			NetworkId.SetNetworkId(GetNextNetworkId());

			UN_LOG(LogFlecsNetworkingManager, Log,
				"Assigned network ID %llu to entity %s",
				NetworkId.GetNetworkId(),
				*Entity.GetEntity().path().c_str());

			if (EntitySyncInfo != nullptr)
			{
				EntitySyncInfo->bInitialized = true;

				
			}
			
		});

		NetworkInitializedObserver = FlecsWorld->CreateObserver<FFlecsNetworkIdComponent, FFlecsEntitySyncInfoComponent>
			(TEXT("NetworkInitializedObserver"))
			.term_at(1)
				.event(flecs::OnAdd)
			.yield_existing(false)
			.term(flecs::Name)
				.and_()
				.inout_none()
			.each([this](const FFlecsEntityHandle& Entity,
				FFlecsNetworkIdComponent& NetworkId, FFlecsEntitySyncInfoComponent& EntitySyncInfo)
			{
				if (EntitySyncInfo.bInitialized)
				{
					return;
				}

				EntitySyncInfo.bInitialized = true;
				
			});
	}

	#endif // WITH_SERVER_CODE
}

void UFlecsNetworkingManager::AddNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent)
{
	checkf(IsValid(NetworkingActorComponent), TEXT("NetworkingActorComponent must be valid"));

	NetworkingActorComponents.Add(NetworkingActorComponent);
}

void UFlecsNetworkingManager::RemoveNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent)
{
	NetworkingActorComponents.Remove(NetworkingActorComponent);
}
