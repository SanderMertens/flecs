// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Entities/FlecsEntityHandle.h"

#include "EntityFunctionLibrary.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UEntityFunctionLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FFlecsId MakePairId(const FFlecsId First, const FFlecsId Second);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool HasEntityFromObject(UObject* Object);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FFlecsEntityHandle GetEntityFromObject(UObject* Object);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void DestroyEntity(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void SetEntityName(const FFlecsEntityHandle& Entity, const FString& EntityName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FString GetEntityName(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void AddComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void AddEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void RemoveComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void RemoveEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool HasComponent(const FFlecsEntityHandle& Entity, const UScriptStruct* ComponentType);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool HasEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsValidEntity(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsAlive(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static bool IsEnabled(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Enable(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Disable(const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static void Clear(const FFlecsEntityHandle& Entity);

}; // class UEntityFunctionLibrary
