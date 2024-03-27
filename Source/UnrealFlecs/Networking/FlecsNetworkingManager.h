// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "GameFramework/Actor.h"
#include "FlecsNetworkingManager.generated.h"

UCLASS(NotPlaceable)
class UNREALFLECS_API AFlecsNetworkingManager final : public AActor
{
	GENERATED_BODY()

public:
	AFlecsNetworkingManager(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

private:
	FFlecsEntityHandle Observer;

	uint64 NextNetworkId = 0;

	FORCEINLINE NO_DISCARD uint64 GetNextNetworkId() { return ++NextNetworkId; }

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;
	
}; // class AFlecsNetworkingManager
