// Fill out your copyright notice in the Description page of Project Settings.

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

	FFlecsComponentPropertiesRegistry::Get()
		.RegisterComponentProperties(StringCast<ANSICHAR>(*ComponentType->GetName()).Get(),
			ComponentPropertiesEntities, ComponentPropertyStructs, false);
}

void UFlecsComponentPropertiesAsset::BeginDestroy()
{
	Super::BeginDestroy();
}
