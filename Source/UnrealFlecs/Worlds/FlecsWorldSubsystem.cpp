// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "Worlds/FlecsWorldSubsystem.h"

#include "Engine/Engine.h"
#include "UnrealEngine.h"
#include "GameplayTagsManager.h"

#include "Logs/FlecsCategories.h"

#include "FlecsWorld.h"
#include "Settings/FlecsWorldSettings.h"
#include "Settings/FlecsWorldSettingsAsset.h"
#include "UnrealFlecsWorldTag.h"

#include "Entities/FlecsDefaultEntitiesDeveloperSettings.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"

#include "Components/FlecsWorldPtrComponent.h"
#include "Components/UWorldPtrComponent.h"

#include "General/FlecsGameplayTagManagerEntity.h"

#include "Modules/FlecsModuleInterface.h"
#include "Modules/FlecsModuleSetDataAsset.h"

#include "Pipelines/FlecsGameLoopInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldSubsystem)

FFlecsOnWorldInitializedGlobal Unreal::Flecs::GOnFlecsWorldInitialized;

bool UFlecsWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer)
		&& IConsoleManager::Get().FindConsoleVariable(TEXT("Flecs.UseFlecs"))->GetBool();
}

void UFlecsWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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

	const UFlecsWorldSettingsAsset* SettingsAsset = SettingsActor->DefaultWorld;
	
	if LIKELY_IF(SettingsActor->bUseFlecsWorld && SettingsAsset)
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

void UFlecsWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if LIKELY_IF(IsValid(DefaultWorld))
	{
		DefaultWorld->WorldBeginPlay();
		OnWorldBeginPlayDelegate.Broadcast(&InWorld);
	}
}

void UFlecsWorldSubsystem::Deinitialize()
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

TStatId UFlecsWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsWorldSubsystem, STATGROUP_Tickables);
}

void UFlecsWorldSubsystem::Tick(float DeltaTime)
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

	UE_CLOG(!bResult, LogFlecsCore, Error, TEXT("Failed to progress Flecs world"));

#endif // WITH_EDITOR
}

UFlecsWorld* UFlecsWorldSubsystem::CreateWorld(const FString& Name, const FFlecsWorldSettingsInfo& Settings)
{
	solid_checkf(!Name.IsEmpty(), TEXT("World name cannot be NAME_None"));

	SetTickableTickType(ETickableTickType::Always);

	const std::vector<FFlecsDefaultMetaEntity>& DefaultEntities = FFlecsDefaultEntityEngine::Get().AddedDefaultEntities;
		
	TMap<FString, FFlecsId> DefaultEntityIds = FFlecsDefaultEntityEngine::Get().DefaultEntityOptions;
		
	// Add a the debug string for this world to the passed-in name e.g. "MyWorld (Client)"
	const FName WorldNameWithWorldContext = FName(Name + " ("+ GetDebugStringForWorld(GetWorld())+")");
		
	DefaultWorld = NewObject<UFlecsWorld>(this, WorldNameWithWorldContext);
	solid_checkf(IsValid(DefaultWorld), TEXT("Failed to create Flecs world"));

	// @TODO: Update this to either the FlecsWorldObject or the UWorld
	DefaultWorld->SetContext(this);

	const TSolidNotNull<UObject*> GameLoop = DuplicateObject<UObject>(Settings.GameLoop, DefaultWorld);

	DefaultWorld->GameLoopInterface = GameLoop;

	DefaultWorld->InitializeComponentPropertyObserver();
	DefaultWorld->InitializeDefaultComponents();

	DefaultWorld->RegisterComponentType<FUnrealFlecsWorldTag>()
	            .Add(flecs::Singleton);
		
	DefaultWorld->RegisterComponentType<FFlecsWorldPtrComponent>()
	            .Add(flecs::Singleton);
		
	DefaultWorld->RegisterComponentType<FUWorldPtrComponent>()
	            .Add(flecs::Singleton);

	DefaultWorld->AddSingleton<FUnrealFlecsWorldTag>();
	
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
#endif // #if !NO_LOGGING
			FFlecsDefaultEntityEngine::Get().CreateDefaultEntity(DefaultEntity, DefaultWorld->World);

		UE_LOGFMT(LogFlecsCore, Log,
		          "Created default entity {EntityName} with id {EntityId}",
		          DefaultEntity.EntityName, NewDefaultEntity.id());
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
		DefaultWorld->SetThreads(FMath::Max(1, FPlatformMisc::NumberOfCores() - 2));
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
	DefaultWorld->bIsInitialized = true;
	OnWorldCreatedDelegate.Broadcast(DefaultWorld);
	Unreal::Flecs::GOnFlecsWorldInitialized.Broadcast(DefaultWorld);
		
	return DefaultWorld;
}

void UFlecsWorldSubsystem::SetWorld(UFlecsWorld* InWorld)
{
	solid_checkf(IsValid(InWorld), TEXT("InWorld cannot be null"));
	solid_checkf(!IsValid(DefaultWorld), TEXT("DefaultWorld is already set"));

	DefaultWorld = InWorld;
}

UFlecsWorld* UFlecsWorldSubsystem::GetDefaultWorld() const
{
	return DefaultWorld;
}

TSolidNotNull<UFlecsWorld*> UFlecsWorldSubsystem::GetDefaultWorldChecked() const
{
	solid_checkf(IsValid(DefaultWorld), TEXT("Default Flecs world is not valid"));
	return DefaultWorld;
}

bool UFlecsWorldSubsystem::HasValidFlecsWorld() const
{
	return IsValid(DefaultWorld);
}

UFlecsWorld* UFlecsWorldSubsystem::GetDefaultWorldStatic(const UObject* WorldContextObject)
{
	solid_check(WorldContextObject);

	const TSolidNotNull<const UWorld*> GameWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	const TSolidNotNull<const UFlecsWorldSubsystem*> FlecsWorldSubsystem = GameWorld->GetSubsystemChecked<UFlecsWorldSubsystem>();
	
	return FlecsWorldSubsystem->DefaultWorld;
}

bool UFlecsWorldSubsystem::HasValidFlecsWorldStatic(const UObject* WorldContextObject)
{
	solid_check(WorldContextObject);
		
	if LIKELY_IF(GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		const TSolidNotNull<const UWorld*> GameWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		const TSolidNotNull<const UFlecsWorldSubsystem*> FlecsWorldSubsystem = GameWorld->GetSubsystemChecked<UFlecsWorldSubsystem>();
		return FlecsWorldSubsystem->HasValidFlecsWorld();
	}
	else
	{
		return false;
	}
}

bool UFlecsWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game
		|| WorldType == EWorldType::PIE
		|| WorldType == EWorldType::GameRPC;
}

void UFlecsWorldSubsystem::ListenBeginPlay(const FFlecsOnWorldBeginPlay::FDelegate& Delegate)
{
	if UNLIKELY_IF(!ensureAlways(IsValid(DefaultWorld)))
	{
		return;
	}

	if (DefaultWorld->HasSingleton<FFlecsBeginPlaySingletonComponent>())
	{
		Delegate.ExecuteIfBound(GetWorld());
	}
	else
	{
		OnWorldBeginPlayDelegate.Add(Delegate);
	}
}

void UFlecsWorldSubsystem::RegisterAllGameplayTags(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld)
{
	InFlecsWorld->ObtainTypedEntity<FFlecsGameplayTagManagerEntity>()
	            .Add(flecs::Module);

	InFlecsWorld->Scope<FFlecsGameplayTagManagerEntity>([InFlecsWorld]()
	{
		FGameplayTagContainer AllTags;
		UGameplayTagsManager::Get().RequestAllGameplayTags(AllTags, false);

		for (const FGameplayTag& Tag : AllTags)
		{
			const FFlecsEntityHandle TagEntity = flecs::entity(InFlecsWorld->World,
			                                                   StringCast<char>(*Tag.ToString()).Get(),
			                                                   ".", ".");
			TagEntity.Set<FGameplayTag>(Tag);
		}
	});
}
