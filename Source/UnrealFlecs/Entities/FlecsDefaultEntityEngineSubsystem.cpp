// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsDefaultEntityEngineSubsystem.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

static constexpr int32 DefaultEntityRangeStart = (EcsFirstUserEntityId * 2);

FFlecsDefaultEntityEngine::FFlecsDefaultEntityEngine()
{
}

void FFlecsDefaultEntityEngine::Initialize()
{
	if (bIsInitialized)
	{
		RefreshDefaultEntities();
		return;
	}
	
	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	checkf(Settings, TEXT("Default Entities Developer Settings not found"));

	#if WITH_EDITOR
	
	Settings->OnSettingChanged().AddLambda([&](UObject*, FPropertyChangedEvent&)
	{
		RefreshDefaultEntities();
	});
	
	#endif // WITH_EDITOR

	RefreshDefaultEntities();
	
	bIsInitialized = true;
}

void FFlecsDefaultEntityEngine::RefreshDefaultEntities()
{
	DefaultEntityWorld = new flecs::world();

	while (DefaultEntityOptions.IsEmpty())
	{
		NoneEntity = DefaultEntityWorld->entity("NoneEntity");
		DefaultEntityWorld->progress();

		DefaultEntityWorld->query_builder<>()
			.without(NoneEntity)
			.with(flecs::Trait).or_()
			.with(flecs::PairIsTag).or_()
			.with_name_component()
			.each([&](flecs::entity Entity)
			{
				if UNLIKELY_IF(Entity.name().length() <= 0)
				{
					return;
				}

				UN_LOGF(LogFlecsEntity, Log,
					"Entity: %s", *FString(Entity.name().c_str()));
				DefaultEntityOptions.Add(Entity.name().c_str(), Entity.id());
			});
	}

	AddedDefaultEntities.Empty();

	for (const FFlecsDefaultMetaEntity& EntityRecord
		: GetDefault<UFlecsDefaultEntitiesDeveloperSettings>()->DefaultEntities)
	{
		if (UNLIKELY(EntityRecord.EntityRecord.Name.IsEmpty()))
		{
			UN_LOG(LogFlecsEntity, Warning,
				"One of the default entities has an empty name");
			continue;
		}

		AddedDefaultEntities.Add(FFlecsDefaultMetaEntity(EntityRecord.EntityRecord));
	}

	for (const FFlecsDefaultMetaEntity& MetaEntity : CodeAddedDefaultEntities)
	{
		AddedDefaultEntities.Add(FFlecsDefaultMetaEntity(MetaEntity.EntityRecord));
	}

	DefaultEntityWorld->progress();
	DefaultEntityWorld->set_entity_range(DefaultEntityRangeStart, 0);

	for (const auto& [EntityRecord] : AddedDefaultEntities)
	{
		UN_LOGF(LogFlecsEntity, Log, "Added default entity: %s", *EntityRecord.Name);

		flecs::entity Entity = DefaultEntityWorld->entity(StringCast<ANSICHAR>(*EntityRecord.Name).Get());
		DefaultEntityOptions.Add(*EntityRecord.Name, Entity);
	}

	delete DefaultEntityWorld;
}

flecs::entity_t FFlecsDefaultEntityEngine::AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity)
{
	CodeAddedDefaultEntities.Add(DefaultEntity);
	RefreshDefaultEntities();
	return DefaultEntityOptions[DefaultEntity.EntityRecord.Name];
}
