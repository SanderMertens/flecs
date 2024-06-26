// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsNetworkingManager.h"
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
		.term_at(1)
			.event(flecs::OnAdd)
			.yield_existing(true)
			.read_write()
		.with_name_component()
			.and_()
			.inout_none()
		.each([this](const flecs::iter& Iterator, const uint64 Index, FFlecsNetworkIdComponent& NetworkId)
		{
			const FFlecsEntityHandle Entity = Iterator.entity(Index);
			
			if UNLIKELY_IF(NetworkId.IsValid())
			{
				return;
			}
			
			NetworkId.SetNetworkId(GetNextNetworkId());

			UN_LOGF(LogFlecsNetworkingManager, Log,
				"Assigned network ID %llu to entity %hc",
				NetworkId.GetNetworkId(),
				*Entity.GetEntity().path());

			
		});
	}

	#endif // WITH_SERVER_CODE
}

void UFlecsNetworkingManager::AddNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent)
{
	solid_checkf(IsValid(NetworkingActorComponent), TEXT("NetworkingActorComponent must be valid"));

	NetworkingActorComponents.Add(NetworkingActorComponent);
}

void UFlecsNetworkingManager::RemoveNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent)
{
	NetworkingActorComponents.Remove(NetworkingActorComponent);
}
