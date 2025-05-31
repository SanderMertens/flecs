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
		FlecsWorld->Defer([this, &FlecsWorld]()
		{
			ModuleEntity = FlecsWorld->CreateEntity(Execute_GetModuleName(_getUObject()));
			
			ModuleEntity.Add(flecs::Module);
			ModuleEntity.SetPairFirst<FFlecsUObjectComponent, FFlecsModuleComponentTag>(FFlecsUObjectComponent{ _getUObject() });
			
			ModuleEntity.Set<FFlecsModuleComponent>({ _getUObject()->GetClass() });
		});

		solid_check(ModuleEntity.IsValid());

		FlecsWorld->SetScope(ModuleEntity);
		
		InitializeModule(FlecsWorld, ModuleEntity);
		Execute_BP_InitializeModule(_getUObject(), FlecsWorld);
	});

	FlecsWorld->Event<FFlecsModuleInitEvent>()
		.id<FFlecsModuleComponent>()
		.entity(ModuleEntity)
		.emit();

	UFlecsWorldSubsystem* WorldSubsystem = GameWorld->GetSubsystem<UFlecsWorldSubsystem>();
	
	WorldSubsystem->ListenBeginPlay(
		FFlecsOnWorldBeginPlay::FDelegate::CreateLambda([this, FlecsWorld](UWorld* InGameWorld)
	{
		WorldBeginPlay(FlecsWorld, InGameWorld);
		Execute_BP_WorldBeginPlay(_getUObject(), FlecsWorld, InGameWorld);
	}));
	
	UN_LOGF(LogFlecsCore, Log,
		"Imported module: %s", *IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
}

void IFlecsModuleInterface::DeinitializeModule_Internal()
{
	ModuleEntity.Disable();

	if (World.IsValid())
	{
		const TSolidNonNullPtr<UFlecsWorld> FlecsWorld = World.Get();
		
		DeinitializeModule(FlecsWorld);
		Execute_BP_DeinitializeModule(_getUObject(), FlecsWorld);
	}

	UN_LOGF(LogFlecsCore, Log,
		"Deinitialized module: %s", *IFlecsModuleInterface::Execute_GetModuleName(_getUObject()));
}

void IFlecsModuleInterface::InitializeModule(TSolidNonNullPtr<UFlecsWorld> InWorld, const FFlecsEntityHandle& InModuleEntity)
{
}

void IFlecsModuleInterface::WorldBeginPlay(TSolidNonNullPtr<UFlecsWorld> InWorld, TSolidNonNullPtr<UWorld> InGameWorld)
{
}

void IFlecsModuleInterface::DeinitializeModule(TSolidNonNullPtr<UFlecsWorld> InWorld)
{
}

FString IFlecsModuleInterface::GetModuleName_Implementation() const
{
	UN_LOGF(LogFlecsCore, Warning,
		"Module name not implemented! It is recommended to implement this function in the module interface. "
		"Will return the inherited class name instead. For Class: %s", *_getUObject()->GetClass()->GetName());
	
	return _getUObject()->GetClass()->GetName();
}
