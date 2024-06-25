// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects

#include "FlecsDefaultEntityEngineSubsystem.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsDefaultEntityEngineSubsystem)

UNLOG_CATEGORY(LogFlecsDefaultEntityEngineSubsystem);

UFlecsDefaultEntityEngineSubsystem::UFlecsDefaultEntityEngineSubsystem()
{
}

void UFlecsDefaultEntityEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const flecs::world DefaultEntityWorld = flecs::world();

	while (DefaultEntityOptions.IsEmpty())
	{
		flecs::entity TestEntity = DefaultEntityWorld.entity("TestEntity");
		DefaultEntityWorld.progress();
		
		DefaultEntityWorld.query_builder<>()
			.without(TestEntity)
			.with(flecs::Trait)
			.or_()
			.with(flecs::PairIsTag)
			.or_()
			.with_name_component()
			.each([&](flecs::entity Entity)
			{
				if (Entity.name().length() <= 0)
				{
					return;
				}
				
				UN_LOG(LogFlecsDefaultEntityEngineSubsystem, Log, "Entity: %s", Entity.name().c_str());
				DefaultEntityOptions.Add(Entity.name().c_str(), Entity.id());
			});
	}

	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	solid_checkf(Settings, TEXT("Default Entities Developer Settings not found"));
	
	Settings->OnSettingChanged().AddLambda([&](UObject*, FPropertyChangedEvent&)
	{
		RefreshDefaultEntities();
	});
	
	RefreshDefaultEntities();
}

void UFlecsDefaultEntityEngineSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UFlecsDefaultEntityEngineSubsystem::RefreshDefaultEntities()
{
	for (const FFlecsDefaultMetaEntity& EntityRecord
		: GetDefault<UFlecsDefaultEntitiesDeveloperSettings>()->DefaultEntities)
	{
		if UNLIKELY_IF(EntityRecord.EntityRecord.Name.IsEmpty())
		{
			continue;
		}

		AddedDefaultEntities.Add(EntityRecord);
	}
}