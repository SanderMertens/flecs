// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsDefaultEntityEngineSubsystem.h"
#include "FlecsDefaultEntitiesDeveloperSettings.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

FFlecsDefaultEntityEngine FFlecsDefaultEntityEngine::Instance;

FFlecsDefaultEntityEngine::FFlecsDefaultEntityEngine()
{
}

FFlecsDefaultEntityEngine::~FFlecsDefaultEntityEngine()
{
	if LIKELY_IF(DefaultEntityWorld)
	{
		DefaultEntityQuery.~query();
		delete DefaultEntityWorld;
		DefaultEntityWorld = nullptr;
	}
}

flecs::entity FFlecsDefaultEntityEngine::CreateDefaultEntity(const FFlecsDefaultMetaEntity& DefaultEntity, flecs::world& World)
{
	flecs::entity Entity = World.make_alive(DefaultEntity.EntityId);
	Entity.set_name(StringCast<char>(*DefaultEntity.EntityName).Get());
	
	if LIKELY_IF(ensureAlwaysMsgf(DefaultEntityFunctions.contains(DefaultEntity.EntityId),
		TEXT("Default entity %s does not have a function registered for it"), *DefaultEntity.EntityName))
	{
		std::invoke(DefaultEntityFunctions.at(DefaultEntity.EntityId), Entity);
	}

	return Entity;
}

void FFlecsDefaultEntityEngine::Initialize()
{
	if (bIsInitialized)
	{
		return;
	}

	DefaultEntityWorld = new flecs::world();
	
	DefaultEntityWorld->progress();
	
	DefaultEntityQuery = DefaultEntityWorld->query_builder<>()
		.without<flecs::Component>()
		.with(flecs::Trait).or_()
		.with(flecs::PairIsTag).or_()
		.with_name_component()
		.build();

	DefaultEntityWorld->progress();

	DefaultEntityQuery.each([this](flecs::entity Entity)
	{
		if UNLIKELY_IF(Entity.name().length() <= 0)
		{
			return;
		}

		UN_LOGF(LogFlecsEntity, Log,
			"Entity: %s", *FString(Entity.name().c_str()));
		DefaultEntityOptions.Add(Entity.name().c_str(), Entity.id());
	});
	
	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	solid_check(IsValid(Settings));

	/*#if WITH_EDITOR
	
	Settings->OnSettingChanged().AddLambda([this](UObject* Object, FPropertyChangedEvent& Event)
	{
		for (const FFlecsDefaultMetaEntity& EntityRecord
			: GetDefault<UFlecsDefaultEntitiesDeveloperSettings>()->DefaultEntities)
		{
			auto ContainsDefaultEntity = [this](const FString& EntityName) -> bool
			{
				for (const FFlecsDefaultMetaEntity& AddedDefaultEntity : AddedDefaultEntities)
				{
					if (AddedDefaultEntity.EntityName == EntityName && AddedDefaultEntity.SetId != 0)
					{
						return true;
					}
				}

				return false;
			};

			if UNLIKELY_IF(EntityRecord.EntityName.IsEmpty())
			{
				UN_LOG(LogFlecsEntity, Warning, "One of the default entities has an empty name");
				continue;
			}

			if (ContainsDefaultEntity(EntityRecord.EntityName))
			{
				continue;
			}

			DefaultEntityWorld->progress();

			const size_t Index = AddedDefaultEntities.size();
			AddedDefaultEntities.emplace_back(FFlecsDefaultMetaEntity(EntityRecord.EntityName, EntityRecord.EntityRecord,
					EntityRecord.EntityId));
			AddedDefaultEntities[Index].SetId = DefaultEntityWorld->make_alive(static_cast<flecs::entity_t>(EntityRecord.EntityId));
			ecs_set_name(*DefaultEntityWorld,
				AddedDefaultEntities[Index].EntityId, StringCast<char>(*EntityRecord.EntityName).Get());

			DefaultEntityWorld->progress();
		}
	});
	
	#endif // WITH_EDITOR*/
	
	bIsInitialized = true;
}

FFlecsId FFlecsDefaultEntityEngine::AddDefaultEntity(FFlecsDefaultMetaEntity DefaultEntity)
{
	auto ContainsDefaultEntity = [this](const FString& EntityName) -> bool
	{
		for (const FFlecsDefaultMetaEntity& AddedDefaultEntity : AddedDefaultEntities)
		{
			if (AddedDefaultEntity.EntityName == EntityName && AddedDefaultEntity.SetId != 0)
			{
				return true;
			}
		}

		return false;
	};

	if (ContainsDefaultEntity(DefaultEntity.EntityName))
	{
		const auto It = std::ranges::find_if(AddedDefaultEntities,
			  [&DefaultEntity](const FFlecsDefaultMetaEntity& Entity) -> bool
			  {
				  return Entity.EntityName == DefaultEntity.EntityName;
			  });

		solid_check(It != AddedDefaultEntities.end());

		return It->SetId;
	}
	
	DefaultEntityWorld->progress();

	CreateDefaultEntity(DefaultEntity, *DefaultEntityWorld);
	
	DefaultEntityWorld->progress();
	
	DefaultEntityOptions.Add(DefaultEntity.EntityName, DefaultEntity.SetId);
	AddedDefaultEntities.emplace_back(DefaultEntity);

	return DefaultEntity.SetId;
}
