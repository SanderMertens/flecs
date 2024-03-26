// Solstice Games © 2024. All Rights Reserved.

#pragma once

// ReSharper disable CppUnusedIncludeDirective

#include <unordered_map>

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsWorld.h"
#include "FlecsWorldSettings.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWorldCreated, FName, UFlecsWorld*);

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
			CreateWorld(DEFAULT_FLECS_WORLD_NAME,
				FFlecsWorldSettings{ static_cast<bool>(DeveloperSettings->bDefaultAutoMerge) });
		}
	}

	virtual void Deinitialize() override
	{
		for (UFlecsWorld* World : Worlds)
		{
			World->DestroyWorld();
		}
		
		Super::Deinitialize();
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsWorldSubsystem, STATGROUP_Tickables);
	}

	virtual void Tick(float DeltaTime) override
	{
		for (const UFlecsWorld* World : Worlds)
		{
			if (World->Progress())
			{
				World->Merge();
			}
		}
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE UFlecsWorld* CreateWorld(const FName& Name, const FFlecsWorldSettings& Settings)
	{
		checkf(!HasWorld(Name), TEXT("World with name %s already exists"), *Name.ToString());
		checkf(Name != NAME_None, TEXT("World name cannot be NAME_None"));

		flecs::world NewWorld = flecs::world();

		UFlecsWorld* NewFlecsWorld = NewObject<UFlecsWorld>(this);
		NewFlecsWorld->SetWorld(std::move(NewWorld));
		
		Worlds.Add(NewFlecsWorld);

		NewFlecsWorld->SetName(Name);
		
		NewFlecsWorld->SetContext(this);

		NewFlecsWorld->SetAutoMerge(Settings.bAutoMerge);

		NewFlecsWorld->SetSingleton<FFlecsWorldPtrComponent>(FFlecsWorldPtrComponent
			{ NewFlecsWorld, GetWorld() });
		
		WorldNameMap.emplace(Name, NewFlecsWorld);

		#if WITH_EDITOR
		
		if (DeveloperSettings->bAutoImportExplorer)
		{
			ImportRestModule(Name, true, FFlecsRestSettings());
		}
		
		#endif // WITH_EDITOR

		GetDefaultWorld(this)->AddSingleton<FFlecsTypeMapComponent>();

		NewFlecsWorld->SetThreads(DeveloperSettings->DefaultWorkerThreads);

		for (UFlecsDefaultEntityEngineSubsystem* DefaultEntityEngineSubsystem
				= GEngine->GetEngineSubsystem<UFlecsDefaultEntityEngineSubsystem>();
			 const TTuple<FName, flecs::entity_t>& Entity : DefaultEntityEngineSubsystem->DefaultEntityMap)
		{
			flecs::entity SpawnedEntity = NewFlecsWorld->World.make_alive(Entity.Value);
			SpawnedEntity.set_name(TCHAR_TO_ANSI(*Entity.Key.ToString()));
		}
		
		OnWorldCreated.Broadcast(Name, NewFlecsWorld);
		
		return Worlds.Last();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE bool HasWorld(const FName& Name) const
	{
		return WorldNameMap.contains(Name);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs | REST API")
	FORCEINLINE void ImportRestModule(const FName& WorldName, const bool bUseMonitoring, const FFlecsRestSettings& Settings) const
	{
		GetFlecsWorld(WorldName)->SetSingleton<flecs::Rest>(
				flecs::Rest { static_cast<uint16>(Settings.Port), TCHAR_TO_ANSI(*Settings.IPAddress) });

		if (bUseMonitoring)
		{
			ImportMonitoringModule(WorldName);
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flecs")
	FORCEINLINE void ImportMonitoringModule(const FName& WorldName) const
	{
		GetFlecsWorld(WorldName)->ImportModule<flecs::monitor>();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE void DestroyWorldByName(const FName& Name)
	{
		DestroyWorld(GetFlecsWorld(Name));
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE void DestroyWorld(UFlecsWorld* World)
	{
		WorldNameMap.erase(World->GetWorldName());

		for (int32 i = 0; i < Worlds.Num(); ++i)
		{
			if (Worlds[i] == World)
			{
				Worlds.RemoveAt(i);
				break;
			}
		}

		World->DestroyWorld();
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE UFlecsWorld* GetFlecsWorld(const FName& Name) const
	{
		checkf(HasWorld(Name), TEXT("World with name %s does not exist"), *Name.ToString());
		return WorldNameMap.at(Name);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE UFlecsWorld* GetWorldStatic(UObject* WorldContextObject, const FName& Name)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(Name);
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE UFlecsWorld* GetDefaultWorld(const UObject* WorldContextObject)
	{
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
		              ->GetSubsystem<UFlecsWorldSubsystem>()->GetFlecsWorld(DEFAULT_FLECS_WORLD_NAME);
	}
	
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE || WorldType == EWorldType::GamePreview
			|| WorldType == EWorldType::GameRPC;
	}
	
	FOnWorldCreated OnWorldCreated;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UFlecsWorld>> Worlds;

	robin_hood::unordered_flat_map<FName, UFlecsWorld*> WorldNameMap;

	UPROPERTY()
	TWeakObjectPtr<const UFlecsDeveloperSettings> DeveloperSettings;
}; // class UFlecsWorldSubsystem
