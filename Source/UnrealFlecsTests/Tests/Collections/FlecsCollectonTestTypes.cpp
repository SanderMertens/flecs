// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectonTestTypes.h"

#include "Collections/FlecsCollectionBuilder.h"

#include "Tests/FlecsTestTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectonTestTypes)

UFlecsCollectionTestClassNoInterface::UFlecsCollectionTestClassNoInterface()
{
}

void UFlecsCollectionTestClassWithInterface::BuildCollection(FFlecsCollectionBuilder& Builder) const
{
	Builder
		.Add<FFlecsTestStruct_Tag_Inherited>();
}
