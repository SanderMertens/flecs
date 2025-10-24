// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Standard/Hashing.h"
#include "Types/SolidNotNull.h"
#include "FlecsWorldInfoSettings.generated.h"

class UFlecsModuleSetDataAsset;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorldSettingsInfo
{
    GENERATED_BODY()

    NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsWorldSettingsInfo& InWorldSettings)
    {
        return GetTypeHash(InWorldSettings.WorldName);
    }
    
public:
    FFlecsWorldSettingsInfo() = default;
    
    FORCEINLINE FFlecsWorldSettingsInfo(const FString& InWorldName, const TSolidNotNull<UObject*> InGameLoop,
        const TArray<TObjectPtr<UObject>>& InModules = {})
        : WorldName(InWorldName)
        , GameLoop(InGameLoop)
        , Modules(InModules)
    {
    }

    NO_DISCARD FORCEINLINE bool operator==(const FFlecsWorldSettingsInfo& Other) const
    {
        return WorldName == Other.WorldName;
    }

    NO_DISCARD FORCEINLINE bool operator!=(const FFlecsWorldSettingsInfo& Other) const
    {
        return !(*this == Other);
    }

    UPROPERTY(EditAnywhere, Category = "World")
    FString WorldName;

    UPROPERTY(EditAnywhere, Instanced, Category = "Game Loop",
        meta = (ObjectMustImplement = "/Script/UnrealFlecs.FlecsGameLoopInterface"))
    TObjectPtr<UObject> GameLoop;

    UPROPERTY(EditAnywhere, Instanced, Category = "Modules",
        meta = (ObjectMustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
    TArray<TObjectPtr<UObject>> Modules;

    UPROPERTY(EditAnywhere, Category = "Modules")
    TArray<TObjectPtr<UFlecsModuleSetDataAsset>> ModuleSets;

#if WITH_EDITORONLY_DATA

    UPROPERTY(EditAnywhere, Instanced, Category = "Editor Modules",
         meta = (ObjectMustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
    TArray<TObjectPtr<UObject>> EditorModules;

    UPROPERTY(EditAnywhere, Category = "Editor Modules")
    TArray<TObjectPtr<UFlecsModuleSetDataAsset>> EditorModuleSets;

#endif // #if WITH_EDITORONLY_DATA
    
}; // struct FFlecsWorldSettingsInfo

DEFINE_STD_HASH(FFlecsWorldSettingsInfo);

