// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsScriptEnumComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptEnumComponent
{
	GENERATED_BODY()

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsScriptEnumComponent& InScriptStructComponent)
	{
		return GetTypeHash(InScriptStructComponent.ScriptEnum);
	}

	FORCEINLINE NO_DISCARD friend bool operator==(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return Lhs.ScriptEnum == Rhs.ScriptEnum;
	}

	FORCEINLINE NO_DISCARD friend bool operator!=(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCEINLINE operator UEnum*() const { return ScriptEnum.Get(); }
    
	FORCEINLINE FFlecsScriptEnumComponent(UEnum* InScriptEnum = nullptr) : ScriptEnum(InScriptEnum) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TWeakObjectPtr<UEnum> ScriptEnum;
    
}; // struct FFlecsScriptEnumComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsScriptEnumComponent, flecs::Sparse);

DEFINE_STD_HASH(FFlecsScriptEnumComponent)
