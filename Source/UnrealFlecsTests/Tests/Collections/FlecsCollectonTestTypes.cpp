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
		.Add<FFlecsTestStruct_Tag>();
}

void UFlecsCollectionTestClassWithInterface_Inherited::BuildCollection(FFlecsCollectionBuilder& Builder) const
{
	Builder
		.Add<FFlecsTestStruct_Tag_Inherited>();
}

void UFlecsCollectionTestClassWithInterface_Parameterized::BuildCollection(FFlecsCollectionBuilder& Builder) const
{
	Builder.Add<FFlecsTestStruct_Tag>()
			.Add<FFlecsTestStruct_Value>()
			.Parameters(FFlecsTestStruct_Value{ 33 },
				[](const FFlecsEntityHandle& Target, const FFlecsTestStruct_Value& Params)
					{
						Target.Set<FFlecsTestStruct_Value>({ Params } );
					}
				);
}

void UFlecsCollectionTestClassWithInterface_WithSubEntities::BuildCollection(FFlecsCollectionBuilder& Builder) const
{
	Builder
		.BeginSubEntity("SubEntity1")
			.Add<FFlecsTestStruct_Tag>()
			.Add<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{1234})
		.EndSubEntity();
}
