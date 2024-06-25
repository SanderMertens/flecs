// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct FFlecsWorldSettings
{
    GENERATED_BODY()

    FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsWorldSettings& InWorldSettings)
    {
        return GetTypeHash(InWorldSettings.WorldName);
    }
    
public:
    FFlecsWorldSettings() = default;

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsWorldSettings& Other) const
    {
        return WorldName == Other.WorldName;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsWorldSettings& Other) const
    {
        return !(*this == Other);
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
    FString WorldName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Flecs", meta = (MustImplement = "UFlecsModuleInterface"))
    TArray<TObjectPtr<UObject>> Modules;
    
}; // struct FFlecsWorldSettings

