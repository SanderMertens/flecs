// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "SolidMacros/Standard/Hashing.h"
#include "FlecsId.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsId
{
    GENERATED_BODY()

    FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsId& InId)
    {
        return GetTypeHash(InId.Id);
    }

    FORCEINLINE constexpr NO_DISCARD static FFlecsId MakePair(const FFlecsId InFirst, const FFlecsId InSecond)
    {
        return FFlecsId(ecs_pair(InFirst, InSecond));
    }

    FORCEINLINE constexpr NO_DISCARD static FFlecsId Null()
    {
        return FFlecsId(flecs::entity::null().id());
    }

public:
    FORCEINLINE FFlecsId() = default;

    FORCEINLINE constexpr FFlecsId(const flecs::id_t InId)
        : Id(InId)
    {
    }

    FORCEINLINE FFlecsId(const flecs::entity InEntity)
        : Id(InEntity.id())
    {
    }

    FORCEINLINE NO_DISCARD bool IsValid() const
    {
        return Id != 0;
    }
    
    FORCEINLINE NO_DISCARD bool operator==(const FFlecsId& Other) const
    {
        return Id == Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator!=(const FFlecsId& Other) const
    {
        return Id != Other.Id;
    }
    
    FORCEINLINE NO_DISCARD bool operator<(const FFlecsId& Other) const
    {
        return Id < Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator>(const FFlecsId& Other) const
    {
        return Id > Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator<=(const FFlecsId& Other) const
    {
        return Id <= Other.Id;
    }

    FORCEINLINE NO_DISCARD bool operator>=(const FFlecsId& Other) const
    {
        return Id >= Other.Id;
    }

    FORCEINLINE NO_DISCARD bool IsPair() const
    {
        return ECS_IS_PAIR(Id);
    }
    
    FORCEINLINE NO_DISCARD bool HasRelation(const FFlecsId InRelation) const
    {
        solid_checkf(IsPair(), TEXT("Id is not a pair."));
        return ECS_HAS_RELATION(Id, InRelation);
    }

    FORCEINLINE NO_DISCARD bool HasTarget(const FFlecsId InTarget) const
    {
        solid_checkf(IsPair(), TEXT("Id is not a pair."));
        return ECS_HAS_RELATION(Id, InTarget);
    }

    FORCEINLINE NO_DISCARD FFlecsId GetFirst() const
    {
        solid_checkf(IsPair(), TEXT("Id is not a pair."));
        return FFlecsId(ECS_PAIR_FIRST(Id));
    }

    FORCEINLINE NO_DISCARD FFlecsId GetSecond() const
    {
        solid_checkf(IsPair(), TEXT("Id is not a pair."));
        return FFlecsId(ECS_PAIR_SECOND(Id));
    }

    FORCEINLINE NO_DISCARD FFlecsId GetRelation() const
    {
        return GetFirst();
    }

    FORCEINLINE NO_DISCARD FFlecsId GetTarget() const
    {
        return GetSecond();
    }

    FORCEINLINE NO_DISCARD flecs::id_t GetId() const
    {
        return Id;
    }

    FORCEINLINE operator flecs::id_t() const
    {
        return GetId();
    }

    FORCEINLINE NO_DISCARD uint32 GetIndex() const
    {
        solid_checkf(!IsPair(), TEXT("Id must not be a pair."));
        return Id & ECS_ENTITY_MASK;
    }

    FORCEINLINE NO_DISCARD uint32 GetGeneration() const
    {
        solid_checkf(!IsPair(), TEXT("Id must not be a pair."));
        return flecs::get_generation(Id);
    }

    FORCEINLINE FString ToString() const
    {
        return FString::Printf(TEXT("Index: %d, Generation: %d"), GetIndex(), GetGeneration());
    }

    FORCEINLINE bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
    {
        uint64 TempId = 0;
        
        if LIKELY_IF(FParse::Value(Buffer, TEXT("FlecsId="), TempId))
        {
            Id = TempId;
            return true;
        }

        return false;
    }

    FORCEINLINE bool ExportTextItem(FString& ValueStr, const FFlecsId& DefaultValue,
        UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
    {
        ValueStr = FString::Printf(TEXT("FlecsId=%llu"), Id);
        return true;
    }

    
    UPROPERTY()
    uint64 Id = 0;
    
}; // struct FFlecsId

template<>
struct TStructOpsTypeTraits<FFlecsId> : public TStructOpsTypeTraitsBase2<FFlecsId>
{
    enum
    {
        WithCopy = true,
        WithIdenticalViaEquality = true,
        WithImportTextItem = true,
        WithExportTextItem = true,
    }; // enum
}; // struct TStructOpsTypeTraits<FFlecsId>

DEFINE_STD_HASH(FFlecsId);
