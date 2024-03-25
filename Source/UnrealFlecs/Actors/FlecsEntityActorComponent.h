// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsEntityActorComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Flecs), meta=(BlueprintSpawnableComponent))
class UNREALFLECS_API UFlecsEntityActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
	FORCEINLINE FFlecsEntityHandle GetEntityHandle() const
	{
		return EntityHandle;
	}

	UPROPERTY(EditAnywhere, Category = "Flecs | Entity")
	FFlecsEntityHandle EntityHandle;
	
}; // class UFlecsEntityActorComponent
