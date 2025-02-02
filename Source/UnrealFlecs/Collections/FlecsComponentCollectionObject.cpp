// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsComponentCollectionObject.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsComponentCollectionObject)

UFlecsComponentCollectionObject::UFlecsComponentCollectionObject()
{
}

UFlecsComponentCollectionObject::UFlecsComponentCollectionObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFlecsComponentCollectionObject::ApplyCollection_Internal(FFlecsEntityHandle Entity, UFlecsWorld* InFlecsWorld)
{
	FlecsWorld = InFlecsWorld;
	
	ApplyCollectionToEntity(Entity);
}

UFlecsWorld* UFlecsComponentCollectionObject::GetFlecsWorld() const
{
	return FlecsWorld.Get();
}

UWorld* UFlecsComponentCollectionObject::GetWorld() const
{
	return GetFlecsWorld()->GetWorld();
}

void UFlecsComponentCollectionObject::ApplyCollectionToEntity_Implementation(FFlecsEntityHandle& Entity)
{
}
