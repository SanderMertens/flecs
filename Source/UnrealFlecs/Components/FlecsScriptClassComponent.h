// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Standard/Hashing.h"
#include "FlecsScriptClassComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptClassComponent
{
    GENERATED_BODY()

    FORCEINLINE friend uint32 GetTypeHash(const FFlecsScriptClassComponent& InScriptClassComponent)
    {
        return GetTypeHash(InScriptClassComponent.ScriptClass);
    }

    FORCEINLINE friend bool operator==(const FFlecsScriptClassComponent& Lhs, const FFlecsScriptClassComponent& Rhs)
    {
        return Lhs.ScriptClass == Rhs.ScriptClass;
    }

    FORCEINLINE friend bool operator!=(const FFlecsScriptClassComponent& Lhs, const FFlecsScriptClassComponent& Rhs)
    {
        return !(Lhs == Rhs);
    }

    FORCEINLINE operator TSubclassOf<UObject>() const { return ScriptClass.Get(); }
    
    FORCEINLINE FFlecsScriptClassComponent(const TSubclassOf<UObject> InScriptClass = nullptr) : ScriptClass(InScriptClass) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    TSubclassOf<UObject> ScriptClass;
}; // struct FFlecsScriptClassComponent

DEFINE_STD_HASH(FFlecsScriptClassComponent)

