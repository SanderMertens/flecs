// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsModuleInterface.h"

#include "Engine/World.h"

#include "Logs/FlecsCategories.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldConverter.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include "Components/FlecsModuleComponent.h"
#include "Components/ObjectTypes/FFlecsModuleObject.h"
#include "Components/FlecsUObjectComponent.h"

#include "Pipelines/FlecsGameLoopInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleInterface)

void IFlecsModuleInterface::ImportModule(const flecs::world& InWorld)
{
	World = Unreal::Flecs::ToUnrealFlecsWorld(InWorld);
	solid_check(IsWorldValid());

	const TSolidNotNull<UFlecsWorld*> FlecsWorld = this->World.Get();
	
	const TSolidNotNull<const UWorld*> GameWorld = FlecsWorld->GetWorld();
	solid_checkf(IsValid(GameWorld), TEXT("FlecsWorld's UWorld is invalid."));

	FlecsWorld->EndScope([this, &FlecsWorld]()
	{
		FlecsWorld->Defer([this, &FlecsWorld]()
		{
			ModuleEntity = FlecsWorld->CreateEntity(Execute_GetModuleName(_getUObject()))
				.Add(flecs::Module)
				.SetPairFirst<FFlecsUObjectComponent, FFlecsModuleObjectTarget>(FFlecsUObjectComponent
				{
					_getUObject()
				})
				.Set<FFlecsModuleComponent>({ _getUObject()->GetClass() });
		});

		solid_check(ModuleEntity.IsValid());

		FlecsWorld->Scope(ModuleEntity, [this, FlecsWorld]()
		{
			InitializeModule(FlecsWorld, ModuleEntity);
			Execute_BP_InitializeModule(_getUObject(), FlecsWorld);
		});
	});

	FlecsWorld->OnModuleImported.Broadcast(ModuleEntity);

	const TSolidNotNull<UFlecsWorldSubsystem*> WorldSubsystem = GameWorld->GetSubsystemChecked<UFlecsWorldSubsystem>();

	WorldSubsystem->ListenBeginPlay(
		FFlecsOnWorldBeginPlay::FDelegate::CreateLambda(
		[this, FlecsWorld](const TSolidNotNull<UWorld*> InGameWorld)
		{
			if UNLIKELY_IF(!IsValid(_getUObject()))
			{
				UE_LOG(LogFlecsCore, Warning,
					TEXT("Module interface is no longer valid when calling WorldBeginPlay for module"));
				return;
			}
			
			if UNLIKELY_IF(!IsWorldValid())
			{
				UE_LOGFMT(LogFlecsCore, Warning,
					"World is no longer valid when calling WorldBeginPlay for module: {ModuleName}",
					IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
				return;
			}
			
			WorldBeginPlay(FlecsWorld, InGameWorld);
			Execute_BP_WorldBeginPlay(_getUObject(), FlecsWorld, InGameWorld);
		}));
	
	UE_LOGFMT(LogFlecsCore, Verbose,
		"Imported module: {ModuleName}", IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));

	TArray<TSubclassOf<UObject>> HardDependencies = GetHardDependentModuleClasses();
	for (const TSubclassOf<UObject> HardDependencyClass : HardDependencies)
	{
		solid_cassumef(HardDependencyClass,
			TEXT("Hard dependency class is null in module: %s"),
			*IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
		
		solid_checkf(HardDependencyClass->ImplementsInterface(UFlecsModuleInterface::StaticClass()),
			TEXT("Hard dependency class does not implement FlecsModuleInterface"));

		FFlecsEntityHandle DependentModuleEntity;
		if (HardDependencyClass->ImplementsInterface(UFlecsGameLoopInterface::StaticClass()))
		{
			DependentModuleEntity = FlecsWorld->GetGameLoopEntity(HardDependencyClass);
		}
		else
		{
			DependentModuleEntity = FlecsWorld->GetModuleEntity(HardDependencyClass);
		}
		
		// since it's a hard dependency, we should never hit this check
		solid_checkf(DependentModuleEntity.IsValid(),
			TEXT("Hard dependency module entity not found for class:  %s Required by module: %s"),
			*HardDependencyClass->GetName(),
			*IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));

		ModuleEntity.AddPair(flecs::DependsOn, DependentModuleEntity);
	}
}

void IFlecsModuleInterface::DeinitializeModule_Internal()
{
	ModuleEntity.Disable();

	if (World.IsValid())
	{
		const TSolidNotNull<UFlecsWorld*> FlecsWorld = World.Get();
		
		DeinitializeModule(FlecsWorld);
		Execute_BP_DeinitializeModule(_getUObject(), FlecsWorld);
	}

	UE_LOGFMT(LogFlecsCore, Log,
		"Deinitialized module: {ModuleName}", IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
}

void IFlecsModuleInterface::InitializeModule(const TSolidNotNull<UFlecsWorld*> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
	 // do nothing
}

void IFlecsModuleInterface::WorldBeginPlay(TSolidNotNull<UFlecsWorld*> InWorld, TSolidNotNull<UWorld*> InGameWorld)
{
	// do nothing
}

void IFlecsModuleInterface::DeinitializeModule(TSolidNotNull<UFlecsWorld*> InWorld)
{
	 // do nothing
}

FString IFlecsModuleInterface::GetModuleName_Implementation() const
{
	/*UE_LOGFMT(LogFlecsCore, Warning,
		"Module name not implemented! It is recommended to implement this function in the module interface. "
		"Will return the inherited class name instead. For Class: {ClassName}", _getUObject()->GetClass()->GetName());*/
	
	return _getUObject()->GetClass()->GetName();
}

TArray<TSubclassOf<UObject>> IFlecsModuleInterface::GetHardDependentModuleClasses() const
{
	return {};
}
