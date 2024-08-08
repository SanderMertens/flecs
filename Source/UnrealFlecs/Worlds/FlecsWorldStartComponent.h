// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlecsWorldStartComponent.generated.h"

class UFlecsWorldSettingsAsset;

UCLASS(ClassGroup=(Flecs), meta=(BlueprintSpawnableComponent))
class UNREALFLECS_API UFlecsWorldStartComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlecsWorldStartComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs", meta = (ExposeOnSpawn = true))
	TObjectPtr<UFlecsWorldSettingsAsset> DefaultWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs", meta = (ExposeOnSpawn = true))
	TArray<TObjectPtr<UFlecsWorldSettingsAsset>> Worlds;


}; // class UFlecsWorldStartComponent
