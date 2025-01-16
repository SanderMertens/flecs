// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsComponentCollectionObject.h"
#include "CollectionTrackerComponent.h"
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

FString UFlecsComponentCollectionObject::GetCollectionName() const
{
	return CollectionName;
}

FFlecsEntityHandle UFlecsComponentCollectionObject::ObtainCollectionEntity(UFlecsWorld* InFlecsWorld)
{
	FlecsWorld = InFlecsWorld;

	CollectionEntity = FlecsWorld->LookupEntity(GetCollectionName());
	
	if (CollectionEntity.IsValid())
	{
		return CollectionEntity;
	}

	CollectionEntity = FlecsWorld->CreateEntity(GetCollectionName());
	CollectionEntity.Add<FFlecsComponentCollection>();
	ApplyCollectionToEntity(CollectionEntity);

	return CollectionEntity;
}

void UFlecsComponentCollectionObject::ApplyCollection_Internal(FFlecsEntityHandle& Entity, UFlecsWorld* InFlecsWorld)
{
	FlecsWorld = InFlecsWorld;

	InFlecsWorld->DeferEndScoped([&Entity, InFlecsWorld]()
	{
		FFlecsEntityHandle ScopedEntity = InFlecsWorld->GetEntity(Entity);
		
		if (!ScopedEntity.Has<FCollectionTrackerComponent>())
		{
			ScopedEntity.Add<FCollectionTrackerComponent>();
		}
	});
	
	FCollectionTrackerComponent* CollectionTracker = Entity.GetPtr<FCollectionTrackerComponent>();
	solid_check(CollectionTracker);
	
	Entity.Add(ObtainCollectionEntity(FlecsWorld));
	CollectionTracker->ComponentProperties.emplace(GetCollectionName(), CollectionEntity);
	
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
