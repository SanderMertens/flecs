// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Entities/FlecsEntityHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Worlds/FlecsWorldSubsystem.h"
#include "DefaultEntitiesLibrary.generated.h"

UCLASS()
class UNREALFLECS_API UDefaultEntitiesLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPairEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Pair));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOverrideEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Override));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetToggleEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Toggle));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetQueryEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Query));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetObserverEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Observer));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPrivateEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Private));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetModuleEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Module));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPrefabEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Prefab));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDisabledEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Disabled));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetEmptyEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetMonitorEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Monitor));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSystemEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::System));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPipelineEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Pipeline));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPhaseEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Phase));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnAddEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::OnAdd));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnRemoveEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::OnRemove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnSetEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::OnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetUnSetEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::UnSet));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnTableCreateEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::OnTableCreate);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnTableDeleteEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::OnTableDelete);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlecsEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Flecs);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlecsCoreEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::FlecsCore);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWorldEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::World);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWildcardEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Wildcard);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAnyEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Any);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetThisEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::This);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTransitiveEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Transitive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetReflexiveEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Reflexive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFinalEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Final);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDontInheritEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::DontInherit);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAlwaysOverrideEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::AlwaysOverride);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTagEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Tag);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetUnionEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Union);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetExclusiveEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Exclusive);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetAcyclicEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Acyclic);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetTraversableEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Traversable);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSymmetricEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::Symmetric);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetWithEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::With);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOneOfEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return flecs::entity(World->World, flecs::OneOf);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetIsAEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::IsA));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetChildOfEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::ChildOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDependsOnEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::DependsOn));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSlotOfEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::SlotOf));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetNameEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Name));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetSymbolEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Symbol));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnDeleteEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::OnDelete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetOnDeleteTargetEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::OnDeleteTarget));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetRemoveEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Remove));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDeleteEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Delete));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPanicEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Panic));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetFlattenEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::Flatten));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetDefaultChildComponentEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::DefaultChildComponent));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateEqualEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::PredEq));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateMatchEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::PredMatch));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetPredicateLookupEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::PredLookup));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetScopeOpenEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::ScopeOpen));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE FFlecsEntityHandle GetScopeCloseEntity(const UObject* WorldContextObject)
	{
		const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
		return FFlecsEntityHandle(flecs::entity(World->World, flecs::ScopeClose));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Default Entities")
	static FORCEINLINE FFlecsEntityHandle GetNullEntity()
	{
		return FFlecsEntityHandle::GetNullHandle();
	}
	
}; // class UDefaultEntitiesLibrary
