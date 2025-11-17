// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"
#include "Standard/Hashing.h"

#include "FlecsCollectionId.generated.h"

/**
 * @brief Struct representing a Flecs Collection Identifier.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionId
{
	GENERATED_BODY()
	
	NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsCollectionId& InId)
	{
    	return GetTypeHash(InId.NameId);
    }

	static NO_DISCARD FORCEINLINE FFlecsCollectionId Make(const FString& InString)
	{
		return FFlecsCollectionId(InString);
	}

public:
	FORCEINLINE FFlecsCollectionId() = default;

	FORCEINLINE FFlecsCollectionId(const FString& InNameId)
		: NameId(InNameId)
	{
	}
	
	FORCEINLINE bool operator==(const FFlecsCollectionId& Other) const
	{
    	return NameId == Other.NameId;
    }
    
    FORCEINLINE bool operator!=(const FFlecsCollectionId& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY(EditAnywhere, Category = "Flecs")
	FString NameId;
	
}; // struct FFlecsCollectionId

DEFINE_STD_HASH(FFlecsCollectionId);
