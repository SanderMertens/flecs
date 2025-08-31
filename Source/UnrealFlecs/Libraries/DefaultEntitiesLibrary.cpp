// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "DefaultEntitiesLibrary.h"

#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DefaultEntitiesLibrary)

FFlecsEntityHandle UDefaultEntitiesLibrary::GetNullEntity(const UObject* WorldContextObject)
{
	solid_check(WorldContextObject);
	
	TNotNull<UFlecsWorld*> FlecsWorld = UFlecsWorld::GetDefaultWorld(WorldContextObject);

	return FFlecsEntityHandle::GetNullHandle(FlecsWorld);
}
