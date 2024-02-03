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
#include "FlecsWorldSubsystem.generated.h"

const FName DEFAULT_FLECS_WORLD_NAME = "DefaultFlecsWorld";

USTRUCT(BlueprintType)
struct FFlecsRestSettings
{
	GENERATED_BODY()

	/* Defaults Set according to flecs's defaults */
	/* no this isn't bad grammar, it's a name */
	
	UPROPERTY(EditAnywhere, Category = "Flecs | REST API",
		meta = (ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
	int32 Port = 27750;

	UPROPERTY(EditAnywhere, Category = "Flecs | REST API")
	FString IPAddress = "0.0.0.0";
}; // struct FFlecsRestSettings

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
		for (FFlecsWorld& World : Worlds)
		{
			if (World->progress(DeltaTime))
			{
				World->merge();
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasScriptStruct(UScriptStruct* ScriptStruct) const
	{
		return ScriptStructMap.Contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasScriptClass(const TSubclassOf<UObject> ScriptClass) const
	{
		return ScriptClassMap.Contains(ScriptClass);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScriptStructEntity(UScriptStruct* ScriptStruct) const
	{
		return ScriptStructMap[ScriptStruct];
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScriptClassEntity(const TSubclassOf<UObject> ScriptClass) const
	{
		return ScriptClassMap[ScriptClass];
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetScriptStructEntity() const
	{
		return GetScriptStructEntity(T::StaticStruct());
	}
	
	template <Solid::TStaticClassConcept T>
	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetScriptClassEntity() const
	{
		return GetScriptClassEntity(T::StaticClass());
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE NO_DISCARD bool HasScriptStruct() const
	{
		return HasScriptStruct(T::StaticStruct());
	}

	template <Solid::TStaticClassConcept T>
	FORCEINLINE NO_DISCARD bool HasScriptClass() const
	{
		return HasScriptClass(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct)
	{
		if UNLIKELY_IF(ScriptStruct == nullptr)
		{
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

		const FFlecsEntityHandle Handle(ScriptStructComponent);
		ScriptStructMap.Add(ScriptStruct, Handle);
		return Handle;
	}

	template <Solid::TStaticStructConcept T>
	FFlecsEntityHandle RegisterScriptStruct()
	{
		return RegisterScriptStruct(T::StaticStruct());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptClass(TSubclassOf<UObject> ScriptClass)
	{
		if UNLIKELY_IF(ScriptClass == nullptr)
		{
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

		const FFlecsEntityHandle Handle(ScriptClassComponent);
		ScriptClassMap.Add(ScriptClass, Handle);
		return Handle;
	}

	template <Solid::TStaticClassConcept T>
	FFlecsEntityHandle RegisterScriptClass()
	{
		return RegisterScriptClass(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterComponentType(const FName& Name, const int32 Size, const int32 Alignment) const
	{
		const flecs::entity Component = GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME)->entity(TCHAR_TO_ANSI(*Name.ToString()))
			.set<flecs::Component>({ Size, Alignment });

		return FFlecsEntityHandle(Component);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& CreateWorld(const FName& Name, const FFlecsWorldSettings& Settings)
	{
		checkf(!HasWorld(Name), TEXT("World with name %s already exists"), *Name.ToString());
		checkf(Name != NAME_None, TEXT("World name cannot be NAME_None"));
		
		flecs::world NewWorld = flecs::world();
		FFlecsWorld NewFlecsWorld(NewWorld);
		
		WorldNameMap[Name] = &Worlds.back();

		NewFlecsWorld.GetFlecsWorld().set<FName>(Name);
		NewFlecsWorld.GetFlecsWorld().set_ctx(this);

		NewFlecsWorld.GetFlecsWorld().set_automerge(Settings.bAutoMerge);

		NewFlecsWorld.GetFlecsWorld().init_builtin_components();

		Worlds.emplace_back(NewFlecsWorld);
		
		return Worlds.back();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasWorld(const FName& Name) const
	{
		return WorldNameMap.contains(Name);
	}

	template <typename T>
	void ImportModule(const FName& WorldName) const
	{
		GetFlecsWorld(WorldName)->import<T>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | REST API")
	void ImportRestModule(const FName& WorldName, const bool bUseMonitoring, const FFlecsRestSettings& Settings) const
	{
		SetSingleton<flecs::Rest>(WorldName,
				{ static_cast<uint16>(Settings.Port), TCHAR_TO_ANSI(*Settings.IPAddress) });

		if (bUseMonitoring)
		{
			ImportMonitoringModule(WorldName);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void ImportMonitoringModule(const FName& WorldName) const
	{
		ImportModule<flecs::monitor>(WorldName);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", BlueprintPure = false)
	void SetAutoMerge(const FName& Name, const bool bAutoMerge) const
	{
		GetFlecsWorld(Name)->set_automerge(bAutoMerge);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyWorldByName(const FName& Name)
	{
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

		delete WorldPtr;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& GetFlecsWorld(const FName& Name) const
	{
		checkf(HasWorld(Name), TEXT("World with name %s does not exist"), *Name.ToString());
		return *WorldNameMap.at(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	static FName GetWorldName(const FFlecsWorld& World)
	{
		return *World->get<FName>();
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FFlecsWorld& GetWorldStatic(UObject* WorldContextObject, const FName& Name)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FFlecsWorld& GetDefaultWorld(const UObject* WorldContextObject)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME);
	}
	
	template <typename T>
	FORCEINLINE void SetSingleton(const FName& WorldName, const T& Value) const
	{
		GetFlecsWorld(WorldName)->set<T>(Value);
	}
	
	template <typename T>
	FORCEINLINE NO_DISCARD T GetSingleton(const FName& WorldName) const
	{
		return GetFlecsWorld(WorldName)->get<T>();
	}
	
	template <typename T>
	FORCEINLINE NO_DISCARD bool HasSingleton(const FName& WorldName) const
	{
		return GetFlecsWorld(WorldName)->has<T>();
	}
	
	template <typename T>
	FORCEINLINE void RemoveSingleton(const FName& WorldName) const
	{
		GetFlecsWorld(WorldName)->remove<T>();
	}
	
	bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

protected:
	std::vector<FFlecsWorld> Worlds;
	std::unordered_map<FName, FFlecsWorld*> WorldNameMap;
	
	TMap<FFlecsScriptStructComponent, FFlecsEntityHandle> ScriptStructMap;
	TMap<FFlecsScriptClassComponent, FFlecsEntityHandle> ScriptClassMap;
}; // class UFlecsWorldSubsystem
