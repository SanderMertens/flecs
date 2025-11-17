// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionEntityRecordFragment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionEntityRecordFragment)

void FFlecsCollectionEntityRecordFragment::PostApplyRecordToEntity(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld,
	const FFlecsEntityHandle& InEntityHandle) const
{
	InEntityHandle.AddCollection(CollectionInstancedReference);
}
