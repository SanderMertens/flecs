// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsComponentPropertiesAsset.h"
#include "FlecsComponentProperties.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsComponentPropertiesAsset)

void UFlecsComponentPropertiesAsset::PostLoad()
{
	Super::PostLoad();

	std::vector<flecs::entity_t> ComponentPropertiesEntities;
	
	for (const FFlecsEntityHandle& ComponentProperty : ComponentProperties)
	{
		ComponentPropertiesEntities.emplace_back(ComponentProperty.GetEntity());
	}

	TArray<FSharedStruct> SharedPropertyStructs;

	for (const FInstancedStruct& PropertyStruct : ComponentPropertyStructs)
	{
		SharedPropertyStructs.Add(FSharedStruct::Make(PropertyStruct.GetScriptStruct(), PropertyStruct.GetMemory()));
	}

	FFlecsComponentPropertiesRegistry::Get()
		.RegisterComponentProperties(StringCast<ANSICHAR>(*ComponentType->GetName()).Get(),
			ComponentPropertiesEntities, SharedPropertyStructs, bResetExistingProperties);
}

void UFlecsComponentPropertiesAsset::BeginDestroy()
{
	Super::BeginDestroy();
}

#if WITH_EDITOR

void UFlecsComponentPropertiesAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	std::vector<flecs::entity_t> ComponentPropertiesEntities;
}

#endif // WITH_EDITOR
