// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityHandle.h"
#include "Subsystems/EngineSubsystem.h"
#include "FlecsDefaultEntityEngineSubsystem.generated.h"

UCLASS()
class UNREALFLECS_API UFlecsDefaultEntityEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void RegisterEntityOption(const FName& EntityName, const flecs::entity_t& EntityHandle);
	
	const TMap<FName, flecs::entity_t>& GetEntityOptions() const;

private:
	TMap<FName, flecs::entity_t> EntityOptionMap;
}; // class UFlecsDefaultEntityEngineSubsystem

#define REGISTER_FLECS_ENTITY_OPTION(EntityName, EntityHandle) \
	GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>()->RegisterEntityOption(FName(EntityName), EntityHandle)
