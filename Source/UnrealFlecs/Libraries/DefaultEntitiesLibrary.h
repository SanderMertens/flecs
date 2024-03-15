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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPairEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pair));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Override));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetToggleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Toggle));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetQueryEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Query));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetObserverEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Observer));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPrivateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Private));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetModuleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Module));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPrefabEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Prefab));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetDisabledEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Disabled));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetEmptyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld()));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetMonitorEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Monitor));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetSystemEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::System));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPipelineEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pipeline));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPhaseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Phase));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnAddEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnAdd));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnRemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnRemove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetUnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::UnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnTableCreateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableCreate);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnTableDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableDelete);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetFlecsEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Flecs);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetFlecsCoreEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::FlecsCore);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetWorldEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::World);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetWildcardEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Wildcard);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetAnyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Any);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetThisEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::This);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetTransitiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Transitive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetReflexiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Reflexive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetFinalEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Final);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetDontInheritEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::DontInherit);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetAlwaysOverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::AlwaysOverride);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetTagEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Tag);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetUnionEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Union);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetExclusiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Exclusive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetAcyclicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Acyclic);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetTraversableEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Traversable);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetSymmetricEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Symmetric);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetWithEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::With);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOneOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OneOf);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetIsAEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::IsA));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetChildOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ChildOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetDependsOnEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DependsOn));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetSlotOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::SlotOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetNameEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Name));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetSymbolEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Symbol));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDelete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetOnDeleteTargetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDeleteTarget));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetRemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Remove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Delete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPanicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Panic));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetFlattenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Flatten));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetDefaultChildComponentEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DefaultChildComponent));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPredicateEqualEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredEq));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPredicateMatchEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredMatch));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetPredicateLookupEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredLookup));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetScopeOpenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeOpen));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FFlecsEntityHandle GetScopeCloseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeClose));
	}
	
}; // class UDefaultEntitiesLibrary
