// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EntityFunctionLibrary.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UEntityFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity", meta = (WorldContext = "WorldContextObject"))
    static FFlecsEntityHandle CreateEntity(const UObject* WorldContextObject, const FString& EntityName = "None");

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity", meta = (WorldContext = "WorldContextObject"))
    static FFlecsEntityHandle GetEntityWithName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void DestroyEntity(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity", meta = (WorldContext = "WorldContextObject"))
    static void DestroyEntityByName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void SetEntityName(const FFlecsEntityHandle& Entity, const FName& EntityName)
    {
        Entity.SetName(EntityName);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FName GetEntityName(const FFlecsEntityHandle& Entity)
    {
        return Entity.GetName();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void AddComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Add(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void AddEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Add(EntityType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void RemoveComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Remove(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void RemoveEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Remove(EntityType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool HasComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        return Entity.Has(ComponentType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool HasEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        return Entity.Has(EntityType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsValid(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsValid();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsAlive(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsAlive();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsEnabled(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsEnabled();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Enable(const FFlecsEntityHandle& Entity)
    {
        Entity.Enable();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Disable(const FFlecsEntityHandle& Entity)
    {
        Entity.Disable();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Clear(const FFlecsEntityHandle& Entity)
    {
        Entity.Clear();
    }
    
}; // class UEntityFunctionLibrary
