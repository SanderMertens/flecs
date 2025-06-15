// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionRegistrationInterface.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionRegistrationInterface)

// Add default functionality here for any IFlecsCollectionRegistrationInterface functions that are not pure virtual.

void IFlecsCollectionRegistrationInterface::RegisterObject(const TSolidNotNull<UFlecsWorld*> InFlecsWorld)
{
	FlecsWorld = InFlecsWorld;
	
	const FFlecsEntityHandle PrefabEntity = InFlecsWorld->CreateEntity().Add(flecs::Prefab);
	solid_check(PrefabEntity.IsValid());
	
	CollectionEntity = PrefabEntity;

	RegisterCollection(InFlecsWorld, CollectionEntity);
}
