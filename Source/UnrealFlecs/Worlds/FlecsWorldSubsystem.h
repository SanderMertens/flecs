// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppUnusedIncludeDirective
#pragma once

#include <thread>
#include <unordered_map>

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsWorld.h"
#include "FlecsWorldInfoSettings.h"
#include "FlecsWorldSettingsAsset.h"
#include "FlecsWorldSettings.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Components/UWorldPtrComponent.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "General/FlecsDeveloperSettings.h"
#include "Modules/FlecsModuleSetDataAsset.h"
#include "Concepts/SolidConcepts.h"
#include "Pipelines/FlecsDefaultGameLoop.h"
#include "Standard/Hashing.h"
#include "Standard/robin_hood.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsOnWorldCreated, TSolidNotNull<UFlecsWorld*>);
DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsOnWorldBeginPlay, TSolidNotNull<UWorld*>);
DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsOnWorldDestroyed, TSolidNotNull<UFlecsWorld*>);

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsWorldSubsystem final : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return Super::ShouldCreateSubsystem(Outer)
			&& IConsoleManager::Get().FindConsoleVariable(TEXT("Flecs.UseFlecs"))->GetBool();
	}
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		
		if (!IConsoleManager::Get().FindConsoleVariable(TEXT("Flecs.UseFlecs"))->GetBool())
		{
			return;
		}

		SetTickableTickType(ETickableTickType::Always);

		solid_check(IsValid(GetWorld()->GetWorldSettings()));
		solid_checkf(GetWorld()->GetWorldSettings()->IsA<AFlecsWorldSettings>(),
			TEXT("World settings must be of type AFlecsWorldSettings"));

		const TSolidNotNull<const AFlecsWorldSettings*> SettingsActor
			= CastChecked<AFlecsWorldSettings>(GetWorld()->GetWorldSettings());

		if (const UFlecsWorldSettingsAsset* SettingsAsset = SettingsActor->DefaultWorld)
		{
			CreateWorld(SettingsAsset->WorldSettings.WorldName, SettingsAsset->WorldSettings);
		}
		else
		{
			#if WITH_EDITOR
			if (!GIsAutomationTesting)
			{
			#endif // #if WITH_EDITOR
				
			UE_LOG(LogFlecsCore, Warning, TEXT("No default world settings asset found"));
				
			#if WITH_EDITOR
			}
			#endif // #if WITH_EDITOR
		}
	}

	virtual void OnWorldBeginPlay(UWorld& InWorld) override
	{
		Super::OnWorldBeginPlay(InWorld);

		if LIKELY_IF(IsValid(DefaultWorld))
		{
			DefaultWorld->WorldBeginPlay();
			OnWorldBeginPlayDelegate.Broadcast(&InWorld);
		}
	}

	virtual void Deinitialize() override
	{
		if (IsValid(DefaultWorld))
		{
			DefaultWorld->RemoveSingleton<FFlecsWorldPtrComponent>();
			DefaultWorld->RemoveSingleton<FUWorldPtrComponent>();
			DefaultWorld->DestroyWorld();
		}

		DefaultWorld = nullptr;

		Super::Deinitialize();
	}

	FORCEINLINE virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsWorldSubsystem, STATGROUP_Tickables);
	}

	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);

		if UNLIKELY_IF(!IsValid(DefaultWorld))
		{
			return;
		}

		#if WITH_EDITOR
		const bool bResult =
		#endif // WITH_EDITOR
			DefaultWorld->ProgressGameLoop(DeltaTime);

		#if WITH_EDITOR

		if UNLIKELY_IF(!bResult)
		{
			UE_LOG(LogFlecsCore, Error, TEXT("Failed to progress Flecs world"));
		}

		#endif // WITH_EDITOR
	}
	
	UFUNCTION()
	FORCEINLINE_DEBUGGABLE UFlecsWorld* CreateWorld(const FString& Name, const FFlecsWorldSettingsInfo& Settings)
	{
		solid_checkf(!Name.IsEmpty(), TEXT("World name cannot be NAME_None"));

		SetTickableTickType(ETickableTickType::Always);

		const std::vector<FFlecsDefaultMetaEntity> DefaultEntities
			= FFlecsDefaultEntityEngine::Get().AddedDefaultEntities;
		
		TMap<FString, FFlecsId> DefaultEntityIds = FFlecsDefaultEntityEngine::Get().DefaultEntityOptions;
		
		// Add a the debug string for this world to the passed-in name e.g. "MyWorld (Client)"
		const FName WorldNameWithWorldContext = FName(Name + " ("+ GetDebugStringForWorld(GetWorld())+")");
		
		DefaultWorld = NewObject<UFlecsWorld>(this, WorldNameWithWorldContext);

		TSolidNotNull<UObject*> GameLoop = DuplicateObject<UObject>(Settings.GameLoop, DefaultWorld);

		DefaultWorld->GameLoopInterface = GameLoop;

		DefaultWorld->InitializeDefaultComponents();

		// @TODO: Update this to either the FlecsWorldObject or the UWorld
		DefaultWorld->SetContext(this);
		
		DefaultWorld->RegisterComponentType<FFlecsWorldPtrComponent>();
		DefaultWorld->RegisterComponentType<FUWorldPtrComponent>();

		DefaultWorld->SetSingleton<FFlecsWorldPtrComponent>(FFlecsWorldPtrComponent{ DefaultWorld });
		DefaultWorld->SetSingleton<FUWorldPtrComponent>(FUWorldPtrComponent{ GetWorld() });

		solid_checkf(DefaultWorld->GetSingleton<FFlecsWorldPtrComponent>().World == DefaultWorld,
			TEXT("Singleton world ptr component does not point to the correct world"));

		DefaultWorld->InitializeSystems();

		RegisterAllGameplayTags(DefaultWorld.Get());
		
		for (const FFlecsDefaultMetaEntity& DefaultEntity : DefaultEntities)
		{
			#if !NO_LOGGING
			flecs::entity NewDefaultEntity =
			#endif // UNLOG_ENABLED
				FFlecsDefaultEntityEngine::Get().CreateDefaultEntity(DefaultEntity, DefaultWorld->World);

			UE_LOG(LogFlecsCore, Log,
				TEXT("Created default entity %s with id %llu"),
				*DefaultEntity.EntityName, NewDefaultEntity.id());
		}

		const IConsoleManager& ConsoleManager = IConsoleManager::Get();

		if (ConsoleManager.FindConsoleVariable(TEXT("Flecs.UseTaskThreads"))->GetBool())
		{
			const TSolidNotNull<IConsoleVariable*> TaskThreads
				= ConsoleManager.FindConsoleVariable(TEXT("Flecs.TaskThreadCount"));
			
			DefaultWorld->SetTaskThreads(TaskThreads->GetInt());
		}
		else
		{
			DefaultWorld->SetThreads(static_cast<int32>(std::thread::hardware_concurrency()));
		}

		DefaultWorld->WorldStart();

		for (TSolidNotNull<UObject*> Module : Settings.Modules)
		{
			solid_check(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()));
			
			DefaultWorld->ImportModule(Module);
		}

		for (const TSolidNotNull<UFlecsModuleSetDataAsset*> ModuleSet : Settings.ModuleSets)
		{
			ModuleSet->ImportModules(DefaultWorld);
		}

		#if WITH_EDITOR

		for (TSolidNotNull<UObject*> Module : Settings.EditorModules)
		{
			solid_checkf(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
				TEXT("Module %s does not implement UFlecsModuleInterface"), *Module->GetName());
			
			DefaultWorld->ImportModule(Module);
		}

		for (const TSolidNotNull<UFlecsModuleSetDataAsset*> ModuleSet : Settings.EditorModuleSets)
		{
			ModuleSet->ImportModules(DefaultWorld);
		}

		#endif // WITH_EDITOR

		DefaultWorld->GameLoopInterface->InitializeGameLoop(DefaultWorld);
		OnWorldCreatedDelegate.Broadcast(DefaultWorld.Get());
		
		return DefaultWorld;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE UFlecsWorld* GetDefaultWorld() const
	{
		return DefaultWorld;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE bool HasValidFlecsWorld() const
	{
		return IsValid(DefaultWorld);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE UFlecsWorld* GetDefaultWorldStatic(const UObject* WorldContextObject)
	{
		solid_check(WorldContextObject);
		
		return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)->
			GetSubsystem<UFlecsWorldSubsystem>()->DefaultWorld;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE bool HasValidFlecsWorldStatic(const UObject* WorldContextObject)
	{
		solid_check(WorldContextObject);
		
		if (GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
		{
			return IsValid(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
			                      ->GetSubsystem<UFlecsWorldSubsystem>()->DefaultWorld);
		}
		else
		{
			return false;
		}
	}

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game
			|| WorldType == EWorldType::PIE
			|| WorldType == EWorldType::GameRPC;
	}

	void ListenBeginPlay(const FFlecsOnWorldBeginPlay::FDelegate& Delegate)
	{
		if UNLIKELY_IF(!ensureAlways(IsValid(DefaultWorld)))
		{
			return;
		}

		if (DefaultWorld->HasSingleton<FFlecsBeginPlay>())
		{
			Delegate.ExecuteIfBound(GetWorld());
		}
		else
		{
			OnWorldBeginPlayDelegate.Add(Delegate);
		}
	}

	FFlecsOnWorldCreated OnWorldCreatedDelegate;
	FFlecsOnWorldBeginPlay OnWorldBeginPlayDelegate;
	FFlecsOnWorldDestroyed OnWorldDestroyedDelegate;

protected:
	UPROPERTY()
	TObjectPtr<UFlecsWorld> DefaultWorld;

	void RegisterAllGameplayTags(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld)
	{
		FGameplayTagContainer AllTags;
		UGameplayTagsManager::Get().RequestAllGameplayTags(AllTags, false);

		for (const FGameplayTag& Tag : AllTags)
		{
			const FFlecsEntityHandle TagEntity = flecs::entity(InFlecsWorld->World,
					Unreal::Flecs::ToCString(Tag.ToString()), ".", ".");
			
			TagEntity.Set<FGameplayTag>(Tag);
		}
	}
	
}; // class UFlecsWorldSubsystem
