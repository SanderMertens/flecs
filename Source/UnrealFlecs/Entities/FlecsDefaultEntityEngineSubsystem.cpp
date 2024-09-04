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

	DefaultEntityWorld = flecs::world();

	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	solid_checkf(Settings, TEXT("Default Entities Developer Settings not found"));

	DEFINE_DEFAULT_ENTITY_OPTION(FlecsNotNetworkedTrait);

	#if WITH_EDITOR
	
	Settings->OnSettingChanged().AddLambda([&](UObject*, FPropertyChangedEvent&)
	{
		RefreshDefaultEntities();
	});

	#endif // WITH_EDITOR
	
	RefreshDefaultEntities();
}

void UFlecsDefaultEntityEngineSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UFlecsDefaultEntityEngineSubsystem::RefreshDefaultEntities()
{
	while (DefaultEntityOptions.IsEmpty())
	{
		TestEntity = DefaultEntityWorld.entity("TestEntity");
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
				if UNLIKELY_IF(Entity.name().length() <= 0)
				{
					return;
				}
				
				UN_LOGF(LogFlecsDefaultEntityEngineSubsystem, Log, "Entity: %s",
					*FString(Entity.name().c_str()));
				DefaultEntityOptions.Add(Entity.name().c_str(), Entity.id());
			});
	}
	
	AddedDefaultEntities.Empty();
	
	for (const FFlecsDefaultMetaEntity& EntityRecord
		: GetDefault<UFlecsDefaultEntitiesDeveloperSettings>()->DefaultEntities)
	{
		if UNLIKELY_IF(EntityRecord.EntityRecord.Name.IsEmpty())
		{
			UN_LOG(LogFlecsDefaultEntityEngineSubsystem, Warning,
				"One of the default entities has an empty name");
			continue;
		}

		AddedDefaultEntities.Add(EntityRecord);
	}

	for (const FFlecsDefaultMetaEntity& EntityRecord : CodeAddedDefaultEntities)
	{
		AddedDefaultEntities.Add(EntityRecord);
	}

	for (const auto& [EntityRecord, bIsOptionEntity] : AddedDefaultEntities)
	{
		UN_LOGF(LogFlecsDefaultEntityEngineSubsystem, Log,
			"Added default entity: %s", *EntityRecord.Name);

		flecs::entity Entity = DefaultEntityWorld.entity(StringCast<ANSICHAR>(*EntityRecord.Name).Get());
		DefaultEntityOptions.Add(*EntityRecord.Name, Entity);
	}
}

flecs::entity_t UFlecsDefaultEntityEngineSubsystem::AddDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity)
{
	CodeAddedDefaultEntities.Add(DefaultEntity);
	RefreshDefaultEntities();
	return DefaultEntityOptions[DefaultEntity.EntityRecord.Name];
}
