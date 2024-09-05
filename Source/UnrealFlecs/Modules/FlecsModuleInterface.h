// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable IdentifierTypo
#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsEntityHandle.h"
#include "SolidMacros/Macros.h"
#include "UObject/Interface.h"
#include "FlecsModuleInterface.generated.h"

class UFlecsWorld;

/**
 * This flecs module impl, doesn't take advantage of scoping in flecs as
 * that creates a lot of problems with the Reflection system.
 * for type impl for modules, you can disable, enable, and toggle the types directly (types will be registered)
 */
// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UNREALFLECS_API UFlecsModuleInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsModuleInterface

/**
 * This flecs module impl, doesn't take advantage of scoping in flecs as
 * that creates a lot of problems with the Reflection system.
 * for type impl for modules, you can disable, enable, and toggle the types directly(types will be registered)
 */
class UNREALFLECS_API IFlecsModuleInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE void ImportModule(const flecs::world& InWorld);
	
	FORCEINLINE void DeinitializeModule_Internal();

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Flecs | Modules", meta = (DisplayName = "Initialize Module"))
	void BP_InitializeModule(UFlecsWorld* InWorld);

	virtual void DeinitializeModule(UFlecsWorld* InWorld);

	UFUNCTION(BlueprintNativeEvent, Category = "Flecs | Modules", meta = (DisplayName = "Deinitialize Module"))
	void BP_DeinitializeModule(UFlecsWorld* InWorld);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Modules")
	FString GetModuleName() const;
	virtual FString GetModuleName_Implementation() const;

	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const
	{
		return World.Get();
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetModuleEntity() const
	{
		return ModuleEntity;
	}
	
	TWeakObjectPtr<UFlecsWorld> World;

	FFlecsEntityHandle ModuleEntity;
	
}; // class IFlecsModuleInterface

/**
 * This flecs module impl, doesn't take advantage of scoping in flecs as
 * that creates a lot of problems with the Reflection system.
 * for type impl for modules, you can disable, enable, and toggle the types directly(types will be registered)
 */
// @TODO Not useable yet
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleStructInterface
{
	GENERATED_BODY()

public:
	virtual ~FFlecsModuleStructInterface();

	FORCEINLINE void ImportModule(const flecs::world& InWorld);

	FORCEINLINE void DeinitializeModule_Internal();

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
		PURE_VIRTUAL(FFlecsModuleStructInterface::InitializeModule, );
	
	virtual void DeinitializeModule(UFlecsWorld* InWorld)
		PURE_VIRTUAL(FFlecsModuleStructInterface::DeinitializeModule, );

	virtual NO_DISCARD FString GetModuleName() const
		PURE_VIRTUAL(FFlecsModuleStructInterface::GetModuleName, return FString(););
	
	FORCEINLINE NO_DISCARD UFlecsWorld* GetFlecsWorld() const;

	FORCEINLINE NO_DISCARD bool IsEnabled() const
	{
		return ModuleEntity.IsEnabled();
	}

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> World;

	UPROPERTY()
	FFlecsEntityHandle ModuleEntity;
	
}; // struct FFlecsModuleInterface

template <>
struct TStructOpsTypeTraits<FFlecsModuleStructInterface> : public TStructOpsTypeTraitsBase2<FFlecsModuleStructInterface>
{
	enum
	{
		WithPureVirtual = true
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsModuleStructInterface>
