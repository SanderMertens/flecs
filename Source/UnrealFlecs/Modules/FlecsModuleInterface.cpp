// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsModuleInterface.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Logs/FlecsCategories.h"
#include "Unlog/Unlog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleInterface)

void IFlecsModuleInterface::ImportModule(const flecs::world& InWorld)
{
	World = ToFlecsWorld(InWorld);
	solid_checkf(World.IsValid(), TEXT("World is not valid!"));

	if (ModuleEntity.IsValid())
	{
		ModuleEntity.Enable();
	}
	else
	{
		ModuleEntity = InWorld.entity(StringCast<char>(*Execute_GetModuleName(_getUObject())).Get());
		solid_checkf(ModuleEntity.IsValid(), TEXT("Module entity is not valid!"));
	}

	ModuleEntity.Add(flecs::Module);
		
	InitializeModule(World.Get(), ModuleEntity);
	Execute_BP_InitializeModule(_getUObject(), World.Get());
}

void IFlecsModuleInterface::DeinitializeModule_Internal()
{
	ModuleEntity.Disable();
	DeinitializeModule(World.Get());
	Execute_BP_DeinitializeModule(_getUObject(), World.Get());
}

void IFlecsModuleInterface::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
}

void IFlecsModuleInterface::DeinitializeModule(UFlecsWorld* InWorld)
{
}

FString IFlecsModuleInterface::GetModuleName_Implementation() const
{
	UN_LOGF(LogFlecsCore, Error,
		"Module name not implemented! It is recommended to implement this function in the module interface. "
		"Will return the inherited class name instead. For Class: %s", *_getUObject()->GetClass()->GetName());
	
	return _getUObject()->GetClass()->GetName();
}

FFlecsModuleStructInterface::~FFlecsModuleStructInterface()
{
}

void FFlecsModuleStructInterface::ImportModule(const flecs::world& InWorld)
{
	World = ToFlecsWorld(InWorld);
	solid_checkf(World.IsValid(), TEXT("World is not valid!"));

	if (ModuleEntity.IsValid())
	{
		ModuleEntity.Enable();
	}
	else
	{
		ModuleEntity = InWorld.entity(StringCast<char>(*GetModuleName()).Get());
		solid_checkf(ModuleEntity.IsValid(), TEXT("Module entity is not valid!"));
	}

	ModuleEntity.Add(flecs::Module);
		
	InitializeModule(World.Get(), ModuleEntity);
}

void FFlecsModuleStructInterface::DeinitializeModule_Internal()
{
	ModuleEntity.Disable();
	DeinitializeModule(World.Get());
}

UFlecsWorld* FFlecsModuleStructInterface::GetFlecsWorld() const
{
	return World.Get();
}
