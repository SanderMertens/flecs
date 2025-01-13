// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Standard/Hashing.h"
#include "FlecsWorldInfoSettings.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorldSettingsInfo
{
    GENERATED_BODY()

    FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsWorldSettingsInfo& InWorldSettings)
    {
        return GetTypeHash(InWorldSettings.WorldName);
    }
    
public:
    FFlecsWorldSettingsInfo() = default;
    
    FORCEINLINE FFlecsWorldSettingsInfo(const FString& InWorldName,
        const TArray<TObjectPtr<UObject>>& InModules = {})
        : WorldName(InWorldName)
        , Modules(InModules)
    {
    }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsWorldSettingsInfo& Other) const
    {
        return WorldName == Other.WorldName;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsWorldSettingsInfo& Other) const
    {
        return !(*this == Other);
    }

    UPROPERTY(EditAnywhere, Category = "Flecs")
    FString WorldName;

    UPROPERTY(EditAnywhere, Instanced, Category = "Flecs",
        meta = (MustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
    TArray<TObjectPtr<UObject>> Modules;

    #if WITH_EDITORONLY_DATA

    UPROPERTY(EditAnywhere, Instanced, Category = "Flecs",
         meta = (MustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
    TArray<TObjectPtr<UObject>> EditorModules;

    #endif // WITH_EDITORONLY_DATA
    
    
}; // struct FFlecsWorldSettingsInfo

DEFINE_STD_HASH(FFlecsWorldSettingsInfo);

