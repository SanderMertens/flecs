// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "UObject/Interface.h"
#include "Worlds/FlecsWorld.h"
#include "FlecsModuleInterface.generated.h"

/**
 * This flecs module impl, doesn't take advantage of scoping in flecs as
 * that creates a lot of problems with the Reflection system.
 * for type impl for modules, you can disable, enable, and toggle the types directly(types will be registered)
 */

// This class does not need to be modified.
UINTERFACE()
class UFlecsModuleInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsModuleInterface

class UNREALFLECS_API IFlecsModuleInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE void ImportModule(const flecs::world& InWorld)
	{
		World = ToFlecsWorld(InWorld);

		const FFlecsEntityHandle ModuleEntity = InWorld.entity(TCHAR_TO_ANSI(*GetModuleName()));
		solid_checkf(ModuleEntity.IsValid(), TEXT("Module entity is not valid!"));

		ModuleEntity.Add(flecs::Module);
		
		InitializeModule(World.Get(), ModuleEntity);
		BP_InitializeModule(World.Get());
	}

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
	{
		
	}
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Modules", meta = (DisplayName = "Initialize Module"))
	void BP_InitializeModule(UFlecsWorld* InWorld);

	virtual void DeinitializeModule(UFlecsWorld* InWorld)
	{
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Modules", meta = (DisplayName = "Deinitialize Module"))
	void BP_DeinitializeModule(UFlecsWorld* InWorld);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Flecs | Modules")
	FString GetModuleName() const;
	FString GetModuleName_Implementation() const PURE_VIRTUAL(IFlecsModuleInterface::GetModuleName, return FString(););

	FORCEINLINE NO_DISCARD UFlecsWorld* GetWorld() const
	{
		return World.Get();
	}

	FORCEINLINE NO_DISCARD flecs::world GetFlecsWorld() const
	{
		return World->World;
	}
	
	TWeakObjectPtr<UFlecsWorld> World;
	
}; // class IFlecsModuleInterface

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsModuleStructInterface
{
	GENERATED_BODY()

public:
	virtual ~FFlecsModuleStructInterface() = default;

	virtual void InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
		PURE_VIRTUAL(FFlecsModuleStructInterface::InitializeModule, );
	virtual void DeinitializeModule(UFlecsWorld* InWorld)
		PURE_VIRTUAL(FFlecsModuleStructInterface::DeinitializeModule, );

	virtual NO_DISCARD FString GetModuleName() const
		PURE_VIRTUAL(FFlecsModuleStructInterface::GetModuleName, return FString(););

	FORCEINLINE NO_DISCARD UFlecsWorld* GetWorld() const
	{
		return World.Get();
	}

	FORCEINLINE NO_DISCARD flecs::world GetFlecsWorld() const
	{
		return World->World;
	}

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> World;
}; // struct FFlecsModuleInterface

template <>
struct TStructOpsTypeTraits<FFlecsModuleStructInterface> : public TStructOpsTypeTraitsBase2<FFlecsModuleStructInterface>
{
	enum
	{
		WithPureVirtual = true
	};
}; // struct TStructOpsTypeTraits<FFlecsModuleStructInterface>
