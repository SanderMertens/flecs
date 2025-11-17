// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"

#include "FlecsCollectionEntityRecordFragment.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionEntityRecordFragment : public FFlecsEntityRecordFragment
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionEntityRecordFragment() = default;

	FORCEINLINE FFlecsCollectionEntityRecordFragment(const FFlecsCollectionInstancedReference& InCollectionInstancedReference)
		: CollectionInstancedReference(InCollectionInstancedReference)
	{
	}
	UPROPERTY(EditAnywhere, Category="Flecs|Entity")
	FFlecsCollectionInstancedReference CollectionInstancedReference;

	virtual void PostApplyRecordToEntity(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld,
		const FFlecsEntityHandle& InEntityHandle) const override;
	
}; // struct FFlecsCollectionEntityRecordFragment
