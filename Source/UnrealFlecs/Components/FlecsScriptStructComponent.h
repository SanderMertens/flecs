// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsScriptStructComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsScriptStructComponent
{
    GENERATED_BODY()

    FORCEINLINE friend uint32 GetTypeHash(const FFlecsScriptStructComponent& InScriptStructComponent)
    {
        return GetTypeHash(InScriptStructComponent.ScriptStruct);
    }

    FORCEINLINE friend bool operator==(const FFlecsScriptStructComponent& Lhs, const FFlecsScriptStructComponent& Rhs)
    {
        return Lhs.ScriptStruct == Rhs.ScriptStruct;
    }

    FORCEINLINE friend bool operator!=(const FFlecsScriptStructComponent& Lhs, const FFlecsScriptStructComponent& Rhs)
    {
        return !(Lhs == Rhs);
    }

    FORCEINLINE operator UScriptStruct*() const { return ScriptStruct.Get(); }

public:
    FORCEINLINE FFlecsScriptStructComponent(UScriptStruct* InScriptStruct = nullptr) : ScriptStruct(InScriptStruct) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    TObjectPtr<UScriptStruct> ScriptStruct;
}; // struct FFlecsScriptStructComponent

template<>
struct std::hash<FFlecsScriptStructComponent>
{
    std::size_t operator()(const FFlecsScriptStructComponent& InScriptStructComponent) const NOEXCEPT
    {
        return GetTypeHash(InScriptStructComponent);
    }
};
