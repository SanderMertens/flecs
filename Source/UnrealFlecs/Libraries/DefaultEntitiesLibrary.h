// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Worlds/FlecsWorldSubsystem.h"
#include "DefaultEntitiesLibrary.generated.h"

UCLASS()
class UNREALFLECS_API UDefaultEntitiesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PairEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pair));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Override));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ToggleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Toggle));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle QueryEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Query));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ObserverEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Observer));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PrivateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Private));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ModuleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Module));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PrefabEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Prefab));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle DisabledEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Disabled));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle EmptyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld()));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle MonitorEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Monitor));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle SystemEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::System));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PipelineEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pipeline));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PhaseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Phase));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnAddEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnAdd));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnRemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnRemove));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnSet));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle UnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::UnSet));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnTableCreateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableCreate);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnTableDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableDelete);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle FlecsEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Flecs);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle FlecsCoreEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::FlecsCore);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle WorldEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::World);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle WildcardEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Wildcard);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle AnyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Any);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ThisEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::This);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle TransitiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Transitive);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ReflexiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Reflexive);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle FinalEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Final);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle DontInheritEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::DontInherit);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle AlwaysOverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::AlwaysOverride);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle TagEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Tag);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle UnionEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Union);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ExclusiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Exclusive);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle AcyclicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Acyclic);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle TraversableEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Traversable);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle SymmetricEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Symmetric);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle WithEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::With);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OneOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OneOf);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle IsAEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::IsA));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ChildOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ChildOf));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle DependsOnEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DependsOn));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle SlotOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::SlotOf));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle NameEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Name));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle SymbolEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Symbol));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDelete));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle OnDeleteTargetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDeleteTarget));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle RemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Remove));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle DeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Delete));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PanicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Panic));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle FlattenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Flatten));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle DefaultChildComponentEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DefaultChildComponent));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PredicateEqualEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredEq));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PredicateMatchEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredMatch));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle PredicateLookupEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredLookup));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ScopeOpenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeOpen));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle ScopeCloseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeClose));
	}
	
}; // class UDefaultEntitiesLibrary
