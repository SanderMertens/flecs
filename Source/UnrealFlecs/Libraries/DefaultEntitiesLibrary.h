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
	static FORCEINLINE FFlecsEntityHandle GetPairEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pair));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Override));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetToggleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Toggle));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetQueryEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Query));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetObserverEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Observer));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPrivateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Private));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetModuleEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Module));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPrefabEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Prefab));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDisabledEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Disabled));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetEmptyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld()));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetMonitorEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Monitor));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSystemEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::System));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPipelineEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Pipeline));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPhaseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Phase));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnAddEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnAdd));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnRemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnRemove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetUnSetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::UnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnTableCreateEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableCreate);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnTableDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OnTableDelete);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlecsEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Flecs);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlecsCoreEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::FlecsCore);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWorldEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::World);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWildcardEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Wildcard);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAnyEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Any);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetThisEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::This);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTransitiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Transitive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetReflexiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Reflexive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFinalEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Final);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDontInheritEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::DontInherit);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAlwaysOverrideEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::AlwaysOverride);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTagEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Tag);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetUnionEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Union);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetExclusiveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Exclusive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAcyclicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Acyclic);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTraversableEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Traversable);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSymmetricEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::Symmetric);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWithEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::With);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOneOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World.GetFlecsWorld(), flecs::OneOf);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetIsAEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::IsA));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetChildOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ChildOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDependsOnEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DependsOn));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSlotOfEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::SlotOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetNameEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Name));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSymbolEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Symbol));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDelete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnDeleteTargetEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::OnDeleteTarget));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetRemoveEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Remove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDeleteEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Delete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPanicEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Panic));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlattenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::Flatten));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDefaultChildComponentEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::DefaultChildComponent));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateEqualEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredEq));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateMatchEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredMatch));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateLookupEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::PredLookup));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetScopeOpenEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeOpen));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetScopeCloseEntity(const UObject* WorldContextObject)
	{
		FFlecsWorld World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World.GetFlecsWorld(), flecs::ScopeClose));
	}
	
}; // class UDefaultEntitiesLibrary
