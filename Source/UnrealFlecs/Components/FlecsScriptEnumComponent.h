// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Standard/Hashing.h"
#include "flecs.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsScriptEnumComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptEnumComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsScriptEnumComponent& InScriptStructComponent)
	{
		return GetTypeHash(InScriptStructComponent.ScriptEnum);
	}

	FORCEINLINE friend NO_DISCARD bool operator==(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return Lhs.ScriptEnum == Rhs.ScriptEnum;
	}

	FORCEINLINE friend NO_DISCARD bool operator!=(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCEINLINE operator UEnum*() const { return ScriptEnum.Get(); }

	FORCEINLINE FFlecsScriptEnumComponent() : ScriptEnum(nullptr) {}
	FORCEINLINE FFlecsScriptEnumComponent(UEnum* InScriptEnum) : ScriptEnum(InScriptEnum) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TWeakObjectPtr<UEnum> ScriptEnum;
    
}; // struct FFlecsScriptEnumComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsScriptEnumComponent, flecs::Sparse);

DEFINE_STD_HASH(FFlecsScriptEnumComponent)
