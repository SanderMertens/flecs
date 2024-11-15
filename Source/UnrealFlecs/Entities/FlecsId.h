// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "SolidMacros/Standard/Hashing.h"
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
    
    FORCEINLINE FFlecsId(const flecs::id InId) : Id(InId) {}
    FORCEINLINE FFlecsId(const flecs::entity InEntity) : Id(InEntity.id()) {}
    FORCEINLINE FFlecsId(const flecs::entity_t InEntity) : Id(InEntity) {}

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
    
    FORCEINLINE bool ImportTextItem(const TCHAR*& Buffer,
        MAYBE_UNUSED int32 PortFlags, MAYBE_UNUSED UObject* Parent,
        FOutputDevice* ErrorText)
    {
        FString Token;
        
        if (!FParse::Token(Buffer, Token, true))
        {
            ErrorText->Logf(TEXT("FFlecsId::ImportTextItem: Failed to parse token from input"));
            return false;
        }
        
        if (Token.StartsWith(TEXT("Id=")))
        {
            const FString IdString = Token.RightChop(3);
            Id = flecs::id(FCString::Strtoui64(*IdString, nullptr, 10));
            return true;
        }
        else
        {
            ErrorText->Logf(TEXT("FFlecsId::ImportTextItem: Invalid format"));
            return false;
        }
    }
    
    FORCEINLINE bool ExportTextItem(FString& ValueStr, const FFlecsId& DefaultValue,
         MAYBE_UNUSED UObject* Parent, MAYBE_UNUSED int32 PortFlags, MAYBE_UNUSED UObject* ExportRootScope) const
    {
        ValueStr = FString::Printf(TEXT("Id=%llu"), Id.raw_id());
        return true;
    }
    
    flecs::id Id;
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
    };
};

DEFINE_STD_HASH(FFlecsId);
