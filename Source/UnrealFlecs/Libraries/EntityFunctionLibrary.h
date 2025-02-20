// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "GameFramework/Actor/FlecsEntityActorComponent.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EntityFunctionLibrary.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UEntityFunctionLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE FFlecsId MakePairId(const FFlecsId First, const FFlecsId Second)
    {
        return FFlecsId::MakePair(First, Second);
    }
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool IsEntityFromObject(UObject* Object)
    {
        if (!IsValid(Object))
        {
            return false;
        }

        if (Object->Implements<UFlecsEntityInterface>())
        {
            return true;
        }

        if (const AActor* Actor = Cast<AActor>(Object))
        {
            return Actor->FindComponentByClass<UFlecsEntityActorComponent>() != nullptr;
        }
        
        return false;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE FFlecsEntityHandle GetEntityFromObject(UObject* Object)
    {
        solid_check(IsValid(Object));

        if (Object->Implements<UFlecsEntityInterface>())
        {
            return CastChecked<IFlecsEntityInterface>(Object)->GetEntityHandle();
        }

        if (const AActor* Actor = Cast<AActor>(Object))
        {
            if (const UFlecsEntityActorComponent* EntityActorComponent
                = Actor->FindComponentByClass<UFlecsEntityActorComponent>())
            {
                return EntityActorComponent->GetEntityHandle();
            }
        }

        return FFlecsEntityHandle::GetNullHandle();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void DestroyEntity(const FFlecsEntityHandle& Entity)
    {
        Entity.Destroy();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void SetEntityName(const FFlecsEntityHandle& Entity, const FString& EntityName)
    {
        Entity.SetName(EntityName);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE FString GetEntityName(const FFlecsEntityHandle& Entity)
    {
        return Entity.GetName();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void AddComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Add(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void AddEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Add(EntityType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void RemoveComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        Entity.Remove(ComponentType);
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void RemoveEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        Entity.Remove(EntityType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool HasComponent(const FFlecsEntityHandle& Entity, UScriptStruct* ComponentType)
    {
        return Entity.Has(ComponentType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool HasEntityType(const FFlecsEntityHandle& Entity, const FFlecsEntityHandle& EntityType)
    {
        return Entity.Has(EntityType);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool IsValidEntity(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsValid();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool IsAlive(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsAlive();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Flecs | Entity")
    static FORCEINLINE bool IsEnabled(const FFlecsEntityHandle& Entity)
    {
        return Entity.IsEnabled();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void Enable(const FFlecsEntityHandle& Entity)
    {
        Entity.Enable();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void Disable(const FFlecsEntityHandle& Entity)
    {
        Entity.Disable();
    }

    UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
    static FORCEINLINE void Clear(const FFlecsEntityHandle& Entity)
    {
        Entity.Clear();
    }
    
}; // class UEntityFunctionLibrary
