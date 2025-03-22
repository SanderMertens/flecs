// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable IdentifierTypo
#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsEntityHandle.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "SolidMacros/Macros.h"
#include "FlecsModuleInterface.generated.h"

class UFlecsWorld;

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UNREALFLECS_API UFlecsModuleInterface : public UFlecsEntityInterface
{
	GENERATED_BODY()
}; // class UFlecsModuleInterface

/**
 * @brief Interface for Flecs modules.
 */
class UNREALFLECS_API IFlecsModuleInterface : public IFlecsEntityInterface
{
	GENERATED_BODY()

public:

	void ImportModule(flecs::world& InWorld);
	
	void DeinitializeModule_Internal();

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Flecs | Modules", meta = (DisplayName = "Initialize Module"))
	void BP_InitializeModule(UFlecsWorld* InWorld);

	virtual void WorldBeginPlay(UFlecsWorld* InWorld, UWorld* InGameWorld);

	UFUNCTION(BlueprintImplementableEvent, Category = "Flecs | Modules", meta = (DisplayName = "World Begin Play"))
	void BP_WorldBeginPlay(UFlecsWorld* InWorld, UWorld* InGameWorld);

	virtual void DeinitializeModule(UFlecsWorld* InWorld);

	UFUNCTION(BlueprintNativeEvent, Category = "Flecs | Modules", meta = (DisplayName = "Deinitialize Module"))
	void BP_DeinitializeModule(UFlecsWorld* InWorld);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Modules")
	FString GetModuleName() const;
	virtual FString GetModuleName_Implementation() const;

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override final
	{
		return ModuleEntity;
	}

	NO_DISCARD FORCEINLINE UFlecsWorld* GetFlecsWorld() const
	{
		return World.Get();
	}

	NO_DISCARD FORCEINLINE bool IsWorldValid() const
	{
		return World.IsValid();
	}

	NO_DISCARD FORCEINLINE FFlecsEntityHandle GetModuleEntity() const
	{
		return ModuleEntity;
	}
	
	TWeakObjectPtr<UFlecsWorld> World;
	FFlecsEntityHandle ModuleEntity;
	
}; // class IFlecsModuleInterface
