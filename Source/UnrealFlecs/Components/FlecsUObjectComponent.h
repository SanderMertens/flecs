// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppRedundantTemplateArguments
#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"

#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"

#include "Properties/FlecsComponentProperties.h"

#include "FlecsUObjectComponent.generated.h"

/**
 * FFlecsUObjectComponent is a component that holds a weak pointer to a UObject.
 * It is used to represent a relationship between an entity and a UObject in the Flecs world,
 * with the target of the relationship being a component type of the type of UObject
 * ex. FFlecsActorComponentTag, FFlecsSceneComponentTag, FFlecsUObjectTag, etc.
 * The owning entity will be automatically destroyed when the UObject is destroyed/garbage collected.
 */
USTRUCT(BlueprintType)
struct alignas(8) UNREALFLECS_API FFlecsUObjectComponent
{
    GENERATED_BODY()

    NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsUObjectComponent& InComponent)
    {
        return GetTypeHash(InComponent.Object);
    }

public:
    FORCEINLINE FFlecsUObjectComponent() = default;
    FORCEINLINE FFlecsUObjectComponent(UObject* InObject) : Object(InObject) {}

    NO_DISCARD FORCEINLINE UObject* GetObject() const
    {
        return Object.Get();
    }

    template <Solid::TStaticClassConcept T>
    NO_DISCARD FORCEINLINE T* GetObject() const
    {
        return Cast<T>(GetObject());
    }

    NO_DISCARD FORCEINLINE TSolidNotNull<UObject*> GetObjectChecked() const
    {
        solid_checkf(IsValid(), TEXT("Object is not valid!"));
        return Object.Get();
    }

    template <Solid::TStaticClassConcept T>
    NO_DISCARD FORCEINLINE TSolidNotNull<T*> GetObjectChecked() const
    {
        return CastChecked<T>(GetObject());
    }
    
    template <Solid::TStaticClassConcept T>
    FORCEINLINE void SetObject(T* InObject)
    {
        Object = InObject;
    }

    template <Solid::TStaticClassConcept T>
    NO_DISCARD FORCEINLINE bool IsA() const
    {
        return Object->IsA<T>();
    }
    
    NO_DISCARD FORCEINLINE bool IsA(const TSolidNotNull<UClass*> InClass) const
    {
        return Object->IsA(InClass);
    }

    NO_DISCARD FORCEINLINE bool IsValid() const
    {
        return Object.IsValid();
    }

    NO_DISCARD FORCEINLINE bool IsValid(const bool bIncludeIfPendingKill, const bool bThreadSafeTest = false) const
    {
        return Object.IsValid(bIncludeIfPendingKill, bThreadSafeTest);
    }
    
    FORCEINLINE operator bool() const
    {
        return IsValid();
    }

    NO_DISCARD FORCEINLINE bool IsStale(const bool bIncludeIfPendingKill = true,
                                        const bool bThreadSafeTest = false) const
    {
        return Object.IsStale(bIncludeIfPendingKill, bThreadSafeTest);
    }

    FORCEINLINE void Reset()
    {
        Object.Reset();
    }

    FORCEINLINE bool operator==(const FFlecsUObjectComponent& Other) const
    {
        return Object == Other.Object;
    }

    FORCEINLINE bool operator!=(const FFlecsUObjectComponent& Other) const
    {
        return !(*this == Other);
    }
    
    UPROPERTY()
    TWeakObjectPtr<UObject> Object;

    NO_DISCARD FORCEINLINE FString ToString() const
    {
        return IsValid() ? Object->GetName() : TEXT("Invalid");
    }
    
}; // struct FFlecsUObjectComponent

REGISTER_FLECS_COMPONENT(FFlecsUObjectComponent,
    [](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
    {
        InComponent
            .Add(flecs::Relationship)
            .Add(flecs::Exclusive)
            .Add(flecs::Acyclic)
            .AddPair(flecs::OnInstantiate, flecs::DontInherit);
    });

// @TODO: Currently not used
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsNoDeleteUObject
{
    GENERATED_BODY()
}; // struct FFlecsNoDeleteUObject

REGISTER_FLECS_COMPONENT(FFlecsNoDeleteUObject,
    [](const flecs::world& InWorld, const FFlecsComponentHandle& InComponent)
    {
        InComponent
            .AddPair(flecs::OnInstantiate, flecs::DontInherit);
    });


