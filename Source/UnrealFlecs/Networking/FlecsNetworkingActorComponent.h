// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeStatic
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
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

private:
	UPROPERTY(ReplicatedUsing=OnRep_NetworkOwnedEntities)
	TArray<uint64> NetworkOwnedEntities;

	UFUNCTION()
	FORCEINLINE void OnRep_NetworkOwnedEntities()
	{
	}

}; // class UFlecsNetworkingActorComponent
