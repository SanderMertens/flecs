// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionRegistrationInterface.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionRegistrationInterface)

void IFlecsCollectionRegistrationInterface::RegisterObject(const TSolidNotNull<UFlecsWorld*> InFlecsWorld)
{
	FlecsWorld = InFlecsWorld;
	
	const FFlecsEntityHandle PrefabEntity = InFlecsWorld->CreatePrefab(_getUObject()->GetClass());
	solid_check(PrefabEntity.IsValid());
	
	CollectionEntity = PrefabEntity;

	RegisterCollection(InFlecsWorld, CollectionEntity);
}
