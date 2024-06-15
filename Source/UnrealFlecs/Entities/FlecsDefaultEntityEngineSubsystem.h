// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsDefaultEntityEngineSubsystem.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsDefaultEntityEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	static constexpr flecs::entity_t StartingDefaultEntityId = FLECS_HI_COMPONENT_ID * 2;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void RegisterEntityOption(const FName& EntityName, const flecs::entity_t& EntityHandle);

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities")
	void RegisterDefaultEntity(const FName& EntityName);
	
	NO_DISCARD const TMap<FName, flecs::entity_t>& GetEntityOptions() const;

	UFUNCTION()
	void UpdateDefaultEntities();
	
	TMap<FName, flecs::entity_t> EntityOptionMap;
	TMap<FName, flecs::entity_t> DefaultEntityMap;
}; // class UFlecsDefaultEntityEngineSubsystem

#define REGISTER_FLECS_ENTITY_OPTION(EntityName, EntityHandle) \
	GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->RegisterEntityOption(EntityName, EntityHandle);
