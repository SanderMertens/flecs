// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Standard/Hashing.h"
#include "FlecsScriptStructComponent.generated.h"

USTRUCT(BlueprintType)
struct FLECSLIBRARY_API FFlecsScriptStructComponent
{
    GENERATED_BODY()

    NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsScriptStructComponent& InScriptStructComponent)
    {
        return GetTypeHash(InScriptStructComponent.ScriptStruct);
    }

    NO_DISCARD FORCEINLINE friend bool operator==(const FFlecsScriptStructComponent& Lhs, const FFlecsScriptStructComponent& Rhs)
    {
        return Lhs.ScriptStruct == Rhs.ScriptStruct;
    }

    NO_DISCARD FORCEINLINE friend bool operator!=(const FFlecsScriptStructComponent& Lhs, const FFlecsScriptStructComponent& Rhs)
    {
        return !(Lhs == Rhs);
    }

    FORCEINLINE operator UScriptStruct*() const { return ScriptStruct.Get(); }
    
    FORCEINLINE FFlecsScriptStructComponent(UScriptStruct* InScriptStruct = nullptr)
        : ScriptStruct(InScriptStruct) {}
    FORCEINLINE FFlecsScriptStructComponent(const UScriptStruct* InScriptStruct)
        : ScriptStruct(const_cast<UScriptStruct*>(InScriptStruct)) {}
    FORCEINLINE FFlecsScriptStructComponent(const FFlecsScriptStructComponent& InScriptStructComponent)
        : ScriptStruct(InScriptStructComponent.ScriptStruct) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    TWeakObjectPtr<UScriptStruct> ScriptStruct;
    
}; // struct FFlecsScriptStructComponent

DEFINE_STD_HASH(FFlecsScriptStructComponent)
