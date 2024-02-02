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

    template <typename ...Args>
    static FFlecsEntityHandle CreateEntity(const UObject* WorldContextObject, Args&&... InArgs);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static FFlecsEntityHandle GetEntityWithName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static void DestroyEntity(const UObject* WorldContextObject, const FFlecsEntityHandle& Entity);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity", meta = (WorldContext = "WorldContextObject"))
    static void DestroyEntityByName(const UObject* WorldContextObject,
        const FString& EntityName, const bool bSearchPath = true);

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static FORCEINLINE bool IsValid(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsValid();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs|Entity")
    static FORCEINLINE bool IsAlive(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsAlive();
    }
    
}; // class UEntityFunctionLibrary
