// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsSystem.h"
#include "Components/FlecsUObjectComponent.h"
#include "UObject/Interface.h"
#include "FlecsSystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UFlecsSystemInterface : public UInterface
{
	GENERATED_BODY()
}; // class UFlecsSystemInterface

class UNREALFLECS_API IFlecsSystemInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE void InitializeSystem_Internal(const flecs::world& InWorld)
	{
		flecs::system_builder Builder(InWorld, StringCast<char>(*GetName()).Get());
		BuildSystem(Builder);

		System = Builder;

		System.GetEntity().Set<FFlecsUObjectComponent>({ _getUObject() });
		
		InitializeSystem();
		BP_InitializeSystem();
	}

	FORCEINLINE virtual void InitializeSystem()
	{
	}

	FORCEINLINE virtual void BuildSystem(flecs::system_builder<>& Builder)
		PURE_VIRTUAL(IFlecsSystemInterface::BuildSystem, return;);

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Initialize System"))
	void BP_InitializeSystem();

	FORCEINLINE NO_DISCARD FFlecsSystem GetSystem() const
	{
		return System;
	}

	FORCEINLINE NO_DISCARD FString GetName() const;
	
	FFlecsSystem System;
	
}; // class IFlecsSystemInterface

template <typename ...TArgs>
struct TFlecsSystemTemplateInterface
{
public:
	virtual ~TFlecsSystemTemplateInterface() = default;

	void InitializeSystemTemplate(const flecs::world& InWorld)
	{
		auto Builder = InWorld.system<TArgs...>();
		BuildSystemTemplate(Builder);
		System = Builder;
	}

	virtual void BuildSystem(flecs::system& Builder) = 0;

	FFlecsSystem System;
}; // struct TFlecsSystemTemplateInterface

