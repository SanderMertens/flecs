// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsId.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsDefaultEntityEngineSubsystem.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsDefaultEntityEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void RegisterEntityOption(const FName& EntityName, const flecs::entity_t& EntityHandle);
	
	const TMap<FName, flecs::entity_t>& GetEntityOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entities", DisplayName = "Register Entity Option", meta = (AdvancedDisplay = "InId"))
	FORCEINLINE void K2_RegisterEntityOption(const FName& EntityName, const FFlecsId& InId = FFlecsId())
	{
		RegisterEntityOption(EntityName, InId.GetFlecsId());
	}

private:
	TMap<FName, flecs::entity_t> EntityOptionMap;
}; // class UFlecsDefaultEntityEngineSubsystem

#define REGISTER_FLECS_ENTITY_OPTION(EntityName, EntityHandle) \
	GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->RegisterEntityOption(FName(EntityName), EntityHandle)

// Must not be a Pre-defined Tag, has to be entirely new
#define REGISTER_FLECS_TAG_OPTION(TagName, TagHandleName) \
	ECS_TAG_DECLARE(TagHandleName); \
	GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->RegisterEntityOption(FName(TagName), TagHandleName)
