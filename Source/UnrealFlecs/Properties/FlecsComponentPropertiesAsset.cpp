// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsComponentPropertiesAsset.h"
#include "FlecsComponentProperties.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsComponentPropertiesAsset)

void UFlecsComponentPropertiesAsset::PostLoad()
{
	Super::PostLoad();

	const std::string_view ComponentTypeName = std::string_view(StringCast<ANSICHAR>(*ComponentType->GetName()).Get());

	std::vector<flecs::entity_t> ComponentPropertiesEntities;
	
	for (const FFlecsEntityHandle& ComponentProperty : ComponentProperties)
	{
		ComponentPropertiesEntities.emplace_back(ComponentProperty.GetEntity());
	}

	FFlecsComponentPropertiesRegistry::Get()
		.RegisterComponentProperties(ComponentTypeName, ComponentPropertiesEntities, ComponentPropertyStructs, false);
}

void UFlecsComponentPropertiesAsset::BeginDestroy()
{
	Super::BeginDestroy();
}
