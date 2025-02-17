// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppUnusedIncludeDirective
#pragma once

#include <thread>
#include <unordered_map>

#include "CoreMinimal.h"
#include "flecs.h"
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
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "SolidMacros/Standard/Hashing.h"
#include "Standard/robin_hood.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWorldCreated, UFlecsWorld*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWorldBeginPlay, UWorld*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWorldDestroyed, UFlecsWorld*);

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
		checkf(GetWorld()->GetWorldSettings()->IsA<AFlecsWorldSettings>(),
			TEXT("World settings must be of type AFlecsWorldSettings"));

		const AFlecsWorldSettings* SettingsActor = CastChecked<AFlecsWorldSettings>(GetWorld()->GetWorldSettings());

		if (const UFlecsWorldSettingsAsset* SettingsAsset = SettingsActor->DefaultWorld)
		{
			CreateWorld(SettingsAsset->WorldSettings.WorldName, SettingsAsset->WorldSettings);
		}
		else
		{
			UN_LOGF(LogFlecsCore, Warning, "No default world settings asset found");
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
		
		const bool bResult = DefaultWorld->Progress(DeltaTime);

		#if WITH_EDITOR

		if UNLIKELY_IF(!bResult)
		{
			UN_LOGF(LogFlecsCore, Error, "Failed to progress Flecs world");
		}

		#endif // WITH_EDITOR
	}
	
	UFUNCTION()
	FORCEINLINE UFlecsWorld* CreateWorld(const FString& Name, const FFlecsWorldSettingsInfo& Settings)
	{
		solid_checkf(!Name.IsEmpty(), TEXT("World name cannot be NAME_None"));

		SetTickableTickType(ETickableTickType::Always);

		const std::vector<FFlecsDefaultMetaEntity> DefaultEntities = FFlecsDefaultEntityEngine::Get().AddedDefaultEntities;
		TMap<FString, FFlecsId> DefaultEntityIds = FFlecsDefaultEntityEngine::Get().DefaultEntityOptions;
		
		// Add a the debug string for this world to the passed-in name e.g. "MyWorld (Client)"
		const FName WorldNameWithWorldContext = FName(Name + " ("+ GetDebugStringForWorld(GetWorld())+")");
		
		DefaultWorld = NewObject<UFlecsWorld>(this, WorldNameWithWorldContext);

		DefaultWorld->InitializeDefaultComponents();
		
		DefaultWorld->RegisterComponentType<FFlecsWorldPtrComponent>();
		DefaultWorld->RegisterComponentType<FUWorldPtrComponent>();
		
		DefaultWorld->SetContext(this);

		DefaultWorld->SetSingleton<FFlecsWorldPtrComponent>(FFlecsWorldPtrComponent{ DefaultWorld });
		DefaultWorld->SetSingleton<FUWorldPtrComponent>(FUWorldPtrComponent{ GetWorld() });

		solid_checkf(DefaultWorld->GetSingletonRef<FFlecsWorldPtrComponent>().World == DefaultWorld,
			TEXT("Singleton world ptr component does not point to the correct world"));
		
		for (const FFlecsDefaultMetaEntity& DefaultEntity : DefaultEntities)
		{
			#if UNLOG_ENABLED
			flecs::entity NewDefaultEntity =
			#endif // UNLOG_ENABLED
				FFlecsDefaultEntityEngine::Get().CreateDefaultEntity(DefaultEntity, DefaultWorld->World);

			UN_LOGF(LogFlecsCore, Log,
				"Created default entity %s with id %d", *DefaultEntity.EntityName, NewDefaultEntity.id());
		}

		const IConsoleManager& ConsoleManager = IConsoleManager::Get();

		if (ConsoleManager.FindConsoleVariable(TEXT("Flecs.UseTaskThreads"))->GetBool())
		{
			const IConsoleVariable* TaskThreads = ConsoleManager.FindConsoleVariable(TEXT("Flecs.TaskThreadCount"));
			solid_checkf(TaskThreads, TEXT("TaskThreads console variable not found!"));
			DefaultWorld->SetTaskThreads(TaskThreads->GetInt());
		}
		else
		{
			DefaultWorld->SetThreads(static_cast<int32>(std::thread::hardware_concurrency()));
		}

		DefaultWorld->WorldStart();

		RegisterAllGameplayTags(DefaultWorld);

		for (UObject* Module : Settings.Modules)
		{
			solid_check(IsValid(Module));
			solid_check(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()));
			
			DefaultWorld->ImportModule(Module);
		}

		#if WITH_EDITOR

		for (UObject* Module : Settings.EditorModules)
		{
			solid_checkf(IsValid(Module), TEXT("Module is not valid!"));
			solid_checkf(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
				TEXT("Module %s does not implement UFlecsModuleInterface"), *Module->GetName());
			
			DefaultWorld->ImportModule(Module);
		}

		#endif // WITH_EDITOR
		
		OnWorldCreatedDelegate.Broadcast(DefaultWorld);
		
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

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE UFlecsWorld* GetDefaultWorldStatic(const UObject* WorldContextObject)
	{
		if LIKELY_IF(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert))
		{
			return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert)
			              ->GetSubsystem<UFlecsWorldSubsystem>()->DefaultWorld;
		}
		else
		{
			return nullptr;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs", Meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE bool HasValidFlecsWorldStatic(const UObject* WorldContextObject)
	{
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

	void ListenBeginPlay(const FOnWorldBeginPlay::FDelegate& Delegate)
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

	FOnWorldCreated OnWorldCreatedDelegate;
	FOnWorldBeginPlay OnWorldBeginPlayDelegate;
	FOnWorldDestroyed OnWorldDestroyedDelegate;

protected:
	UPROPERTY()
	TObjectPtr<UFlecsWorld> DefaultWorld;

	void RegisterAllGameplayTags(const UFlecsWorld* InFlecsWorld)
	{
		FGameplayTagContainer AllTags;
		UGameplayTagsManager::Get().RequestAllGameplayTags(AllTags, false);

		for (const FGameplayTag& Tag : AllTags)
		{
			const FFlecsEntityHandle TagEntity =
				flecs::entity(InFlecsWorld->World, StringCast<char>(*Tag.ToString()).Get(), ".", ".");
			TagEntity.Set<FGameplayTag>(Tag);
		}
	}
	
}; // class UFlecsWorldSubsystem
