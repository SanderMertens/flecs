// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsId.generated.h"

USTRUCT(BlueprintType)
struct FFlecsId final
{
    GENERATED_BODY()

    FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsId& InId)
    {
        return GetTypeHash(InId.Id.raw_id());
    }

public:
    FORCEINLINE FFlecsId() = default;
    FORCEINLINE FFlecsId(const flecs::id& InId) : Id(InId) {}
    FORCEINLINE FFlecsId(const flecs::id* InId) : Id(*InId) {}

    FORCEINLINE void SetId(const flecs::id& InId) { Id = InId; }
    FORCEINLINE void SetId(const flecs::id* InId) { Id = *InId; }

    FORCEINLINE NO_DISCARD flecs::id& GetFlecsId() { return Id; }
    FORCEINLINE NO_DISCARD const flecs::id& GetFlecsId() const { return Id; }
    
    FORCEINLINE operator flecs::id() const { return GetFlecsId(); }
    

    FORCEINLINE operator flecs::id&() { return GetFlecsId(); }
    FORCEINLINE operator const flecs::id&() const { return GetFlecsId(); }
    
    FORCEINLINE NO_DISCARD flecs::id* operator->() { return &Id; }
    FORCEINLINE NO_DISCARD const flecs::id* operator->() const { return &Id; }

    FORCEINLINE NO_DISCARD bool IsPair() const { return Id.is_pair(); }

    FORCEINLINE NO_DISCARD bool operator==(const FFlecsId& Other) const
    {
        return Id == Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsId& Other) const
    {
        return Id != Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator==(const flecs::id& Other) const
    {
        return Id == Other;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const flecs::id& Other) const
    {
        return Id != Other;
    }

    FORCEINLINE NO_DISCARD bool operator==(const flecs::id* Other) const
    {
        return Id == *Other;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const flecs::id* Other) const
    {
        return Id != *Other;
    }

    FORCEINLINE NO_DISCARD flecs::entity GetTypeId() const { return Id.type_id(); }
    
    flecs::id Id;
}; // struct FFlecsId

template<>
struct TStructOpsTypeTraits<FFlecsId> : public TStructOpsTypeTraitsBase2<FFlecsId>
{
    enum
    {
        WithCopy = true,
        WithIdenticalViaEquality = true
        E
    };
};
