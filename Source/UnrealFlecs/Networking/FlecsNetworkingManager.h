// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsNetworkIdComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "GameFramework/Actor.h"
#include "FlecsNetworkingManager.generated.h"

class UFlecsNetworkingActorComponent;

UENUM()
enum class EFlecsNetworkingEvent : uint8
{
	None = 0,
	
	NetworkIdAssigned,
	NetworkIdRemoved,
	NetworkInitialized,

	
}; // enum class EFlecsNetworkingEvent

USTRUCT()
struct FFlecsNetworkEntityInitData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FFlecsNetworkIdComponent NetworkId;

	UPROPERTY()
	TArray<FInstancedStruct> ComponentData;
	
}; // struct FFlecsNetworkEntityInitData

UCLASS(NotPlaceable)
class UNREALFLECS_API UFlecsNetworkingManager final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlecsNetworkingManager(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	void AddNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent);
	void RemoveNetworkingActorComponent(UFlecsNetworkingActorComponent* NetworkingActorComponent);

private:
	UPROPERTY()
	FFlecsEntityHandle NetworkIdObserver;

	UPROPERTY()
	FFlecsEntityHandle NetworkInitializedObserver;

	UPROPERTY()
	uint64 NextNetworkId = 0;

	FORCEINLINE NO_DISCARD uint64 GetNextNetworkId() { return ++NextNetworkId; }

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;

	UPROPERTY()
	TArray<TWeakObjectPtr<UFlecsNetworkingActorComponent>> NetworkingActorComponents;
	
}; // class AFlecsNetworkingManager
