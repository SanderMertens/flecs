// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Standard/Hashing.h"
#include "FlecsScriptEnumComponent.generated.h"

USTRUCT(BlueprintType)
struct FLECSLIBRARY_API FFlecsScriptEnumComponent
{
	GENERATED_BODY()

	NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsScriptEnumComponent& InScriptEnumComponent)
	{
		return GetTypeHash(InScriptEnumComponent.ScriptEnum);
	}

	NO_DISCARD FORCEINLINE friend bool operator==(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return Lhs.ScriptEnum == Rhs.ScriptEnum;
	}

	NO_DISCARD FORCEINLINE friend bool operator!=(const FFlecsScriptEnumComponent& Lhs, const FFlecsScriptEnumComponent& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCEINLINE operator UEnum*() const { return ScriptEnum.Get(); }
    
	FORCEINLINE FFlecsScriptEnumComponent(UEnum* InScriptEnum = nullptr)
		: ScriptEnum(InScriptEnum) {}
	FORCEINLINE FFlecsScriptEnumComponent(const UEnum* InScriptEnum)
		: ScriptEnum(const_cast<UEnum*>(InScriptEnum)) {}
	FORCEINLINE FFlecsScriptEnumComponent(const FFlecsScriptEnumComponent& InScriptEnumComponent)
		: ScriptEnum(InScriptEnumComponent.ScriptEnum) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TWeakObjectPtr<UEnum> ScriptEnum;
    
}; // struct FFlecsScriptEnumComponent

DEFINE_STD_HASH(FFlecsScriptEnumComponent)
