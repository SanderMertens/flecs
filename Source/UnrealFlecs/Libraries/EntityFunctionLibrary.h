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

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static FFlecsEntityHandle CreateEntity(const UObject* WorldContextObject, const FString& EntityName);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static FFlecsEntityHandle GetEntityWithName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static void DestroyEntity(const UObject* WorldContextObject, const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static void DestroyEntityByName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static void SetEntityName(const FFlecsEntityHandle& Entity, const FName& EntityName)
    {
        Entity.SetName(EntityName);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static FName GetEntityName(const FFlecsEntityHandle& Entity)
    {
        return Entity.GetName();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static void AddComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Add(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static void AddEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Add(EntityType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static void RemoveComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Remove(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static void RemoveEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Remove(EntityType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static bool IsValid(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsValid();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static bool IsAlive(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsAlive();
    }
    
}; // class UEntityFunctionLibrary
