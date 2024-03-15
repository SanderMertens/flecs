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
#include "Entities/FlecsEntityHandle.h"
#include "General/FlecsDeveloperSettings.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "SolidMacros/Standard/Hashing.h"
#include "Standard/robin_hood.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
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
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return GetDefault<UFlecsDeveloperSettings>() && GetDefault<UFlecsDeveloperSettings>()->bEnableFlecs;
	}

	virtual ETickableTickType GetTickableTickType() const override
	{
		return GetDefault<UFlecsDeveloperSettings>()
			&& GetDefault<UFlecsDeveloperSettings>()->bAutoTickWorld ? ETickableTickType::Always : ETickableTickType::Never;
	}
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
	}

	virtual void OnWorldBeginPlay(UWorld& InWorld) override
	{
		DeveloperSettings = GetDefault<UFlecsDeveloperSettings>();

		checkf(DeveloperSettings->bEnableFlecs, TEXT("Flecs is not enabled in the Flecs Developer Settings"));
		
		if (DeveloperSettings->bAutoCreateWorld)
		{
			CreateWorld(DEFAULT_FLECS_WORLD_NAME, FFlecsWorldSettings());
		}
	}

	virtual void Deinitialize() override
	{
		Super::Deinitialize();

		if (!DeveloperSettings.IsValid())
		{
			return;
		}

		if (DeveloperSettings->bAutoDestroyWorld)
		{
			DestroyWorldByName(DEFAULT_FLECS_WORLD_NAME);
		}
	}

	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

	virtual void Tick(float DeltaTime) override
	{
		for (FFlecsWorld& World : Worlds)
		{
			if (World.GetFlecsWorld().progress(DeltaTime))
			{
				World.GetFlecsWorld().merge();
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasScriptStruct(UScriptStruct* ScriptStruct) const
	{
		return ScriptStructMap.contains(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasScriptClass(const TSubclassOf<UObject> ScriptClass) const
	{
		return ScriptClassMap.contains(ScriptClass);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScriptStructEntity(UScriptStruct* ScriptStruct) const
	{
		return ScriptStructMap.at(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle GetScriptClassEntity(const TSubclassOf<UObject> ScriptClass) const
	{
		return ScriptClassMap.at(ScriptClass);
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
	FFlecsEntityHandle RegisterScriptStruct(UScriptStruct* ScriptStruct) const
	{
		if UNLIKELY_IF(ScriptStruct == nullptr)
		{
			return FFlecsEntityHandle();
		}
		
		const flecs::entity ScriptStructComponent
			= GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME).GetFlecsWorld().entity(TCHAR_TO_ANSI(*ScriptStruct->GetFName().ToString()))
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
		ScriptStructMap.emplace(ScriptStruct, Handle);
		return Handle;
	}

	template <Solid::TStaticStructConcept T>
	FFlecsEntityHandle RegisterScriptStruct() const
	{
		return RegisterScriptStruct(T::StaticStruct());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterScriptClass(TSubclassOf<UObject> ScriptClass) const
	{
		if UNLIKELY_IF(ScriptClass == nullptr)
		{
			return FFlecsEntityHandle();
		}
		
		const flecs::entity ScriptClassComponent
			= GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME).GetFlecsWorld().entity(TCHAR_TO_ANSI(*ScriptClass->GetFName().ToString()))
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
		ScriptClassMap.emplace(ScriptClass, Handle);
		return Handle;
	}

	template <Solid::TStaticClassConcept T>
	FFlecsEntityHandle RegisterScriptClass() const
	{
		return RegisterScriptClass(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle RegisterComponentType(const FName& Name, const int32 Size, const int32 Alignment) const
	{
		const flecs::entity Component = GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME).GetFlecsWorld().entity(TCHAR_TO_ANSI(*Name.ToString()))
			.set<flecs::Component>({ Size, Alignment });

		return FFlecsEntityHandle(Component);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle ObtainComponentTypeStruct(UScriptStruct* ScriptStruct) const
	{
		if (HasScriptStruct(ScriptStruct))
		{
			return GetScriptStructEntity(ScriptStruct);
		}

		return RegisterScriptStruct(ScriptStruct);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsEntityHandle ObtainComponentTypeClass(const TSubclassOf<UObject> ScriptClass) const
	{
		if (HasScriptClass(ScriptClass))
		{
			return GetScriptClassEntity(ScriptClass);
		}

		return RegisterScriptClass(ScriptClass);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FFlecsWorld& CreateWorld(const FName& Name, const FFlecsWorldSettings& Settings)
	{
		checkf(!HasWorld(Name), TEXT("World with name %s already exists"), *Name.ToString());
		checkf(Name != NAME_None, TEXT("World name cannot be NAME_None"));
		
		flecs::world* NewWorld = new flecs::world();
		FFlecsWorld NewFlecsWorld(NewWorld);
		
		WorldNameMap.emplace(Name, &NewFlecsWorld);

		// Worlds have a Name Singleton
		NewFlecsWorld.GetFlecsWorld().set<FName>(GWorld_Name_Component, Name);
		
		NewFlecsWorld.GetFlecsWorld().set_ctx(this);

		NewFlecsWorld.GetFlecsWorld().set_automerge(Settings.bAutoMerge);

		Worlds.emplace_back(NewFlecsWorld);

		if (DeveloperSettings->bAutoImportExplorer)
		{
			ImportRestModule(Name, true, FFlecsRestSettings());
		}
		
		return Worlds.back();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	bool HasWorld(const FName& Name) const
	{
		return WorldNameMap.contains(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs | REST API")
	void ImportRestModule(const FName& WorldName, const bool bUseMonitoring, const FFlecsRestSettings& Settings) const
	{
		GetFlecsWorld(WorldName).GetFlecsWorld().set<flecs::Rest>(
				flecs::Rest { static_cast<uint16>(Settings.Port), TCHAR_TO_ANSI(*Settings.IPAddress) });

		if (bUseMonitoring)
		{
			ImportMonitoringModule(WorldName);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void ImportMonitoringModule(const FName& WorldName) const
	{
		GetFlecsWorld(WorldName).ImportModule<flecs::monitor>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyWorldByName(const FName& Name)
	{
		DestroyWorld(GetFlecsWorld(Name));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	void DestroyWorld(FFlecsWorld& World)
	{
		const FFlecsWorld* WorldPtr = WorldNameMap.at(*World.GetFlecsWorld().get<FName>());
		WorldNameMap.erase(*World.GetFlecsWorld().get<FName>());

		for (std::vector<FFlecsWorld>::iterator It = Worlds.begin(); It != Worlds.end(); ++It)
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
		return *World.GetFlecsWorld().get<FName>(GWorld_Name_Component);
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
	
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

protected:
	std::vector<FFlecsWorld> Worlds;
	robin_hood::unordered_flat_map<FName, FFlecsWorld*> WorldNameMap;
	
	mutable robin_hood::unordered_flat_map<FFlecsScriptStructComponent, FFlecsEntityHandle> ScriptStructMap;
	mutable robin_hood::unordered_flat_map<FFlecsScriptClassComponent, FFlecsEntityHandle> ScriptClassMap;

	UPROPERTY()
	TWeakObjectPtr<const UFlecsDeveloperSettings> DeveloperSettings;
}; // class UFlecsWorldSubsystem
