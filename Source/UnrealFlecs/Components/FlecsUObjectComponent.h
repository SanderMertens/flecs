// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppRedundantTemplateArguments
#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "SolidMacros/Macros.h"
#include "UObject/Object.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsUObjectComponent.generated.h"

USTRUCT(BlueprintType)
struct alignas(8) UNREALFLECS_API FFlecsUObjectComponent
{
    GENERATED_BODY()

    FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsUObjectComponent& InComponent)
    {
        return GetTypeHash(InComponent.Object);
    }

public:
    FORCEINLINE FFlecsUObjectComponent() = default;
    FORCEINLINE FFlecsUObjectComponent(UObject* InObject) : Object(InObject) {}

    FORCEINLINE NO_DISCARD UObject* GetObject() const
    {
        return Object.Get();
    }

    template <typename T>
    FORCEINLINE NO_DISCARD T* GetObject() const
    {
        return Cast<T>(GetObject());
    }

    FORCEINLINE NO_DISCARD UObject* GetObjectChecked() const
    {
        solid_checkf(IsValid(), TEXT("Object is not valid!"));
        return Object.Get();
    }

    template <typename T>
    FORCEINLINE NO_DISCARD T* GetObjectChecked() const
    {
        return CastChecked<T>(GetObject());
    }
    
    template <Solid::TStaticClassConcept T>
    FORCEINLINE void SetObject(T* InObject)
    {
        Object = InObject;
    }

    template <Solid::TStaticClassConcept T>
    FORCEINLINE NO_DISCARD bool IsA() const
    {
        return Object->IsA<T>();
    }
    
    FORCEINLINE NO_DISCARD bool IsA(const UClass* InClass) const
    {
        return Object->IsA(InClass);
    }

    FORCEINLINE NO_DISCARD bool IsValid() const
    {
        return Object.IsValid();
    }

    FORCEINLINE NO_DISCARD bool IsValid(const bool bIncludeIfPendingKill, const bool bThreadSafeTest = false) const
    {
        return Object.IsValid(bIncludeIfPendingKill, bThreadSafeTest);
    }
    
    FORCEINLINE operator bool() const
    {
        return IsValid();
    }
    
    FORCEINLINE NO_DISCARD bool IsStale(const bool bIncludeIfPendingKill = true, const bool bThreadSafeTest = false) const
    {
        return Object.IsStale(bIncludeIfPendingKill, bThreadSafeTest);
    }

    FORCEINLINE void Reset()
    {
        Object = nullptr;
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

    FORCEINLINE NO_DISCARD FString ToString() const
    {
        return IsValid() ? Object->GetName() : TEXT("Invalid");
    }
    
}; // struct FFlecsUObjectComponent

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsUObjectComponent, ecs_pair(flecs::OnInstantiate, flecs::DontInherit));

UNREALFLECS_API DECLARE_DEFAULT_ENTITY(DontDeleteUObjectEntity);


