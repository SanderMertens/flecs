// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsTransformComponents.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformComponents)

UFlecsTransform3dCollection::UFlecsTransform3dCollection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Location = FFlecsLocationComponent();
	Rotation = FFlecsRotationComponent();
	Scale = FFlecsScaleComponent();
}

void UFlecsTransform3dCollection::ApplyCollectionToEntity_Implementation(FFlecsEntityHandle& Entity)
{
	Super::ApplyCollectionToEntity_Implementation(Entity);
	
	Entity.Set<FFlecsLocationComponent>(Location);
	Entity.Set<FFlecsRotationComponent>(Rotation);
	Entity.Set<FFlecsScaleComponent>(Scale);
}
