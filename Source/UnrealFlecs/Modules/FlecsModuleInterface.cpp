// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#include "FlecsModuleInterface.h"
#include "FlecsModuleInitEvent.h"
#include "Components/FlecsModuleComponent.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Components/ObjectTypes/FFlecsModuleComponentTag.h"
#include "Logs/FlecsCategories.h"
#include "Unlog/Unlog.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleInterface)

void IFlecsModuleInterface::ImportModule(flecs::world& InWorld)
{
	World = ToFlecsWorld(InWorld);
	solid_check(World.IsValid());

	UFlecsWorld* FlecsWorld = this->World.Get();

	const UWorld* GameWorld = FlecsWorld->GetWorld();

	FlecsWorld->EndScope([this, &FlecsWorld]()
	{
		ModuleEntity = FlecsWorld->CreateEntity(Execute_GetModuleName(_getUObject()));
		solid_check(ModuleEntity.IsValid());
		
		ModuleEntity.Add(flecs::Module);
		ModuleEntity.Set<FFlecsUObjectComponent>({ _getUObject() });
		ModuleEntity.AddTrait<FFlecsUObjectComponent, FFlecsModuleComponentTag>();
		
		ModuleEntity.Set<FFlecsModuleComponent>({ _getUObject()->GetClass() });

		FlecsWorld->SetScope(ModuleEntity);
		
		InitializeModule(FlecsWorld, ModuleEntity);
		Execute_BP_InitializeModule(_getUObject(), FlecsWorld);
	});

	FlecsWorld->Event<FFlecsModuleInitEvent>()
		.id<FFlecsModuleComponent>()
		.entity(ModuleEntity)
		.emit();

	if (FlecsWorld->GetWorldEntity().Has<FFlecsBeginPlay>())
	{
		WorldBeginPlay(World.Get(), FlecsWorld->GetWorld());
		Execute_BP_WorldBeginPlay(_getUObject(), World.Get(), FlecsWorld->GetWorld());
	}
	else
	{
		UFlecsWorldSubsystem* WorldSubsystem = GameWorld->GetSubsystem<UFlecsWorldSubsystem>();
		WorldSubsystem->OnWorldBeginPlayDelegate.AddWeakLambda(_getUObject(), [this](UWorld* InGameWorld)
		{
			UFlecsWorld* NewFlecsWorld = World.Get();
			WorldBeginPlay(NewFlecsWorld, InGameWorld);
			Execute_BP_WorldBeginPlay(_getUObject(), NewFlecsWorld, InGameWorld);
		});
	}
	
	UN_LOGF(LogFlecsCore, Log,
		"Imported module: %s", *IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
}

inline void IFlecsModuleInterface::DeinitializeModule_Internal()
{
	ModuleEntity.Disable();

	if (World.IsValid())
	{
		UFlecsWorld* FlecsWorld = World.Get();
		
		DeinitializeModule(FlecsWorld);
		Execute_BP_DeinitializeModule(_getUObject(), FlecsWorld);
	}

	UN_LOGF(LogFlecsCore, Log, "Deinitialized module: %s",
		*IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
}

void IFlecsModuleInterface::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
}

void IFlecsModuleInterface::WorldBeginPlay(UFlecsWorld* InWorld, UWorld* InGameWorld)
{
}

void IFlecsModuleInterface::DeinitializeModule(UFlecsWorld* InWorld)
{
}

FString IFlecsModuleInterface::GetModuleName_Implementation() const
{
	UN_LOGF(LogFlecsCore, Warning,
		"Module name not implemented! It is recommended to implement this function in the module interface. "
		"Will return the inherited class name instead. For Class: %s", *_getUObject()->GetClass()->GetName());
	
	return _getUObject()->GetClass()->GetName();
}
