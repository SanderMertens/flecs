// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsDefaultEntityEngineSubsystem.h"

#include "FlecsDefaultEntitiesDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsDefaultEntityEngineSubsystem)

void UFlecsDefaultEntityEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	REGISTER_FLECS_ENTITY_OPTION("Empty", flecs::Empty);
	REGISTER_FLECS_ENTITY_OPTION("Pair", flecs::Pair);
	REGISTER_FLECS_ENTITY_OPTION("Override", flecs::Override);
	REGISTER_FLECS_ENTITY_OPTION("Toggle", flecs::Toggle);
	REGISTER_FLECS_ENTITY_OPTION("Query", flecs::Query);
	REGISTER_FLECS_ENTITY_OPTION("Observer", flecs::Observer);
	REGISTER_FLECS_ENTITY_OPTION("Private", flecs::Private);
	REGISTER_FLECS_ENTITY_OPTION("Module", flecs::Module);
	REGISTER_FLECS_ENTITY_OPTION("Prefab", flecs::Prefab);
	REGISTER_FLECS_ENTITY_OPTION("Disabled", flecs::Disabled);
	REGISTER_FLECS_ENTITY_OPTION("Monitor", flecs::Monitor);
	REGISTER_FLECS_ENTITY_OPTION("System", flecs::System);
	REGISTER_FLECS_ENTITY_OPTION("Pipeline", flecs::Pipeline);
	REGISTER_FLECS_ENTITY_OPTION("Phase", flecs::Phase);
	REGISTER_FLECS_ENTITY_OPTION("OnAdd", flecs::OnAdd);
	REGISTER_FLECS_ENTITY_OPTION("OnRemove", flecs::OnRemove);
	REGISTER_FLECS_ENTITY_OPTION("OnSet", flecs::OnSet);
	REGISTER_FLECS_ENTITY_OPTION("UnSet", flecs::UnSet);
	REGISTER_FLECS_ENTITY_OPTION("OnTableCreate", flecs::OnTableCreate);
	REGISTER_FLECS_ENTITY_OPTION("OnTableDelete", flecs::OnTableDelete);
	REGISTER_FLECS_ENTITY_OPTION("Flecs", flecs::Flecs);
	REGISTER_FLECS_ENTITY_OPTION("FlecsCore", flecs::FlecsCore);
	REGISTER_FLECS_ENTITY_OPTION("World", flecs::World);
	REGISTER_FLECS_ENTITY_OPTION("Wildcard", flecs::Wildcard);
	REGISTER_FLECS_ENTITY_OPTION("Any", flecs::Any);
	REGISTER_FLECS_ENTITY_OPTION("This", flecs::This);
	REGISTER_FLECS_ENTITY_OPTION("Transitive", flecs::Transitive);
	REGISTER_FLECS_ENTITY_OPTION("Reflexive", flecs::Reflexive);
	REGISTER_FLECS_ENTITY_OPTION("Final", flecs::Final);
	REGISTER_FLECS_ENTITY_OPTION("DontInherit", flecs::DontInherit);
	REGISTER_FLECS_ENTITY_OPTION("AlwaysOverride", flecs::AlwaysOverride);
	REGISTER_FLECS_ENTITY_OPTION("Tag", flecs::Tag);
	REGISTER_FLECS_ENTITY_OPTION("Union", flecs::Union);
	REGISTER_FLECS_ENTITY_OPTION("Exclusive", flecs::Exclusive);
	REGISTER_FLECS_ENTITY_OPTION("Acyclic", flecs::Acyclic);
	REGISTER_FLECS_ENTITY_OPTION("Traversable", flecs::Traversable);
	REGISTER_FLECS_ENTITY_OPTION("Symmetric", flecs::Symmetric);
	REGISTER_FLECS_ENTITY_OPTION("With", flecs::With);
	REGISTER_FLECS_ENTITY_OPTION("OneOf", flecs::OneOf);
	REGISTER_FLECS_ENTITY_OPTION("IsA", flecs::IsA);
	REGISTER_FLECS_ENTITY_OPTION("ChildOf", flecs::ChildOf);
	REGISTER_FLECS_ENTITY_OPTION("DependsOn", flecs::DependsOn);
	REGISTER_FLECS_ENTITY_OPTION("SlotOf", flecs::SlotOf);
	REGISTER_FLECS_ENTITY_OPTION("Name", flecs::Name);
	REGISTER_FLECS_ENTITY_OPTION("Symbol", flecs::Symbol);
	REGISTER_FLECS_ENTITY_OPTION("OnDelete", flecs::OnDelete);
	REGISTER_FLECS_ENTITY_OPTION("OnDeleteTarget", flecs::OnDeleteTarget);
	REGISTER_FLECS_ENTITY_OPTION("Remove", flecs::Remove);
	REGISTER_FLECS_ENTITY_OPTION("Delete", flecs::Delete);
	REGISTER_FLECS_ENTITY_OPTION("Panic", flecs::Panic);
	REGISTER_FLECS_ENTITY_OPTION("Flatten", flecs::Flatten);
	REGISTER_FLECS_ENTITY_OPTION("DefaultChildComponent", flecs::DefaultChildComponent);
	REGISTER_FLECS_ENTITY_OPTION("PredEq", flecs::PredEq);
	REGISTER_FLECS_ENTITY_OPTION("PredMatch", flecs::PredMatch);
	REGISTER_FLECS_ENTITY_OPTION("PredLookup", flecs::PredLookup);
	REGISTER_FLECS_ENTITY_OPTION("ScopeOpen", flecs::ScopeOpen);
	REGISTER_FLECS_ENTITY_OPTION("ScopeClose", flecs::ScopeClose);
	REGISTER_FLECS_ENTITY_OPTION("AndFrom", flecs::Union);
	REGISTER_FLECS_ENTITY_OPTION("OrFrom", flecs::Exclusive);

	UFlecsDefaultEntitiesDeveloperSettings* Settings = GetMutableDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	checkf(Settings, TEXT("Default Entities Developer Settings not found"));
	UpdateDefaultEntities();

	#if WITH_EDITOR
	
	Settings->OnSettingChanged().AddLambda([this](UObject*, FPropertyChangedEvent&)
	{
		UpdateDefaultEntities();
	});

	#endif // WITH_EDITOR
}

void UFlecsDefaultEntityEngineSubsystem::RegisterEntityOption(const FName& EntityName,
                                                              const flecs::entity_t& EntityHandle)
{
	EntityOptionMap.Emplace(EntityName, EntityHandle);
}

const TMap<FName, flecs::entity_t>& UFlecsDefaultEntityEngineSubsystem::GetEntityOptions() const
{
	return EntityOptionMap;
}

void UFlecsDefaultEntityEngineSubsystem::UpdateDefaultEntities()
{
	const UFlecsDefaultEntitiesDeveloperSettings* Settings = GetDefault<UFlecsDefaultEntitiesDeveloperSettings>();
	checkf(Settings, TEXT("Default Entities Developer Settings not found"));
	
	DefaultEntityMap.Empty();

	for (size_t Index = 0; Index < Settings->DefaultEntities.Num(); ++Index)
	{
		DefaultEntityMap.Emplace(Settings->DefaultEntities[Index], StartingDefaultEntityId + Index + 1);
	}

	for (const TTuple<FName, flecs::entity_t>& Entity : DefaultEntityMap)
	{
		EntityOptionMap.Emplace(Entity.Key, Entity.Value);
	}
}
