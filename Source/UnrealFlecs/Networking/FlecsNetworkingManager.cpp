// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsNetworkingManager.h"
#include "NetworkIdComponent.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsNetworkingManager)

UNLOG_CATEGORY(LogFlecsNetworkingManager);

AFlecsNetworkingManager::AFlecsNetworkingManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	NetUpdateFrequency = 1.0f;
	bAlwaysRelevant = true;
}

void AFlecsNetworkingManager::BeginPlay()
{
	Super::BeginPlay();

	FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(this);

	if (HasAuthority())
	{
		Observer = FlecsWorld->CreateObserver<FFlecsNetworkIdComponent>(TEXT("NetworkingObserver"))
		.event(flecs::OnAdd)
		.yield_existing(true)
		.each([this](const FFlecsEntityHandle& Entity, FFlecsNetworkIdComponent& NetworkId)
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
		});
	}
}
