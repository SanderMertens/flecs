// Solstice Games © 2024. All Rights Reserved.

#pragma once

// ReSharper disable CppUnusedIncludeDirective

#include <unordered_map>

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsWorld.h"
#include "FlecsWorldSettings.h"
#include "Components/FlecsScriptClassComponent.h"
#include "Components/FlecsScriptStructComponent.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "SolidMacros/Standard/Hashing.h"
#include "Subsystems/WorldSubsystem.h"
#include "General/UnLog/Unlog.h"
#include "FlecsWorldSubsystem.generated.h"

UNLOG_CATEGORY(FlecsWorldSubsystemLogCategory)

const FName DEFAULT_FLECS_WORLD_NAME = "DefaultFlecsWorld";

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorldSubsystem final : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);

		CreateWorld(DEFAULT_FLECS_WORLD_NAME, FFlecsWorldSettings());
	}

	void Deinitialize() override
	{
		Super::Deinitialize();

		DestroyWorldByName(DEFAULT_FLECS_WORLD_NAME);
	}

	void Tick(float DeltaTime) override
	{
		
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct) const
	{
		if UNLIKELY_IF(ScriptStruct == nullptr)
		{
			UN_LOG(FlecsWorldSubsystemLogCategory, Error, "UFlecsWorldSubsystem::RegisterScriptStruct: ScriptStruct is nullptr");
			return FFlecsEntityHandle();
		}
		
		const flecs::entity ScriptStructComponent
			= GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME)->entity(TCHAR_TO_ANSI(*ScriptStruct->GetFName().ToString()))
			                                         .set<flecs::Component>(
			                                         {
				                                         ScriptStruct->GetStructureSize(),
				                                         ScriptStruct->GetMinAlignment()
			                                         })
			                                         .set<FFlecsScriptStructComponent>(
			                                         {
				                                         ScriptStruct
			                                         });

		return FFlecsEntityHandle(ScriptStructComponent);
	}

	template <Solid::TStaticStructConcept T>
	FFlecsEntityHandle RegisterScriptStruct()
	{
		return RegisterScriptStruct(T::StaticStruct());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptClass(TSubclassOf<UObject> ScriptClass) const
	{
		if UNLIKELY_IF(ScriptClass == nullptr)
		{
			UN_LOG(FlecsWorldSubsystemLogCategory, Error, "UFlecsWorldSubsystem::RegisterScriptClass: ScriptClass is nullptr");
			return FFlecsEntityHandle();
		}
		
		const flecs::entity ScriptClassComponent
			= GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME)->entity(TCHAR_TO_ANSI(*ScriptClass->GetFName().ToString()))
			                                        .set<flecs::Component>(
			                                        {
				                                        ScriptClass->GetStructureSize(),
				                                        ScriptClass->GetMinAlignment()
			                                        })
			                                        .set<FFlecsScriptClassComponent>(
			                                        {
				                                        ScriptClass
			                                        });

		return FFlecsEntityHandle(ScriptClassComponent);
	}

	template <Solid::TStaticClassConcept T>
	FFlecsEntityHandle RegisterScriptClass()
	{
		return RegisterScriptClass(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterComponent(const FName& Name, const int32 Size, const int32 Alignment) const
	{
		const flecs::entity Component = GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME)->entity(TCHAR_TO_ANSI(*Name.ToString()))
			.set<flecs::Component>({ Size, Alignment });

		return FFlecsEntityHandle(Component);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& CreateWorld(const FName& Name, const FFlecsWorldSettings& Settings)
	{
		flecs::world NewWorld = flecs::world();
		FFlecsWorld World(NewWorld);
		
		Worlds.emplace_back(World);
		WorldNameMap[Name] = &Worlds.back();

		World->set<FName>(Name);
		World->set_ctx(this);

		World->set_automerge(Settings.bAutoMerge);
		
		return Worlds.back();
	}

	template <typename T>
	void ImportModule(const FName& WorldName) const
	{
		GetFlecsWorld(WorldName)->import<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", BlueprintPure = false)
	void SetAutoMerge(const FName& Name, const bool bAutoMerge) const
	{
		GetFlecsWorld(Name)->set_automerge(bAutoMerge);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& GetFlecsWorld(const FName& Name) const
	{
		return *WorldNameMap.at(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FFlecsWorld& GetWorldStatic(UObject* WorldContextObject, const FName& Name)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FFlecsWorld& GetDefaultWorld(const UObject* WorldContextObject)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyWorldByName(const FName& Name)
	{
		WorldNameMap.at(Name)->GetWorld().init_builtin_components();
		WorldNameMap.erase(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyWorld(FFlecsWorld& World)
	{
		const FFlecsWorld* WorldPtr = WorldNameMap.at(*World->get<FName>());
		WorldNameMap.erase(*World->get<FName>());

		for (auto It = Worlds.begin(); It != Worlds.end(); ++It)
		{
			if (&*It == WorldPtr)
			{
				Worlds.erase(It);
				break;
			}
		}
	}
	
	bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

protected:
	std::vector<FFlecsWorld> Worlds;
	std::unordered_map<FName, FFlecsWorld*> WorldNameMap;
}; // class UFlecsWorldSubsystem
