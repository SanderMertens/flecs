// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "flecs.h"
#include "Standard/robin_hood.h"
#include "SolidMacros/Macros.h"
#include "InstancedStruct.h"
#include "Unlog/Unlog.h"

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string_view Name;
	std::vector<flecs::entity_t> Entities;
	
	TArray<FInstancedStruct> ComponentPropertyStructs;
}; // struct FFlecsComponentProperties

DECLARE_DELEGATE_OneParam(FOnComponentPropertiesRegistered, FFlecsComponentProperties);

struct UNREALFLECS_API FFlecsComponentPropertiesRegistry final
{
public:
	static FFlecsComponentPropertiesRegistry& Get()
	{
		static FFlecsComponentPropertiesRegistry Instance;
		return Instance;
	}

	FORCEINLINE void RegisterComponentProperties(const std::string_view& Name, const std::vector<flecs::entity_t>& Entities,
		const TArray<FInstancedStruct>& ComponentPropertyStructs, const bool bResetExisting = false)
	{
		UNLOG_CATEGORY_SCOPED(LogFlecsComponentProperties);

		if (bResetExisting)
		{
			if (ComponentProperties.contains(Name))
			{
				ComponentProperties.erase(Name);
			}
		}
		
		if (!ComponentProperties.contains(Name))
		{
			ComponentProperties[Name] = FFlecsComponentProperties{ Name, Entities,
				ComponentPropertyStructs };
			
			UN_LOGF(LogFlecsComponentProperties, Log,
				"Registered component properties: %s", *FString(Name.data()));
		}
		else
		{
			for (const flecs::entity_t& Entity : Entities)
			{
				if (std::ranges::find(ComponentProperties[Name].Entities, Entity)
					!= ComponentProperties[Name].Entities.end())
				{
					continue;
				}

				ComponentProperties[Name].Entities.emplace_back(Entity);

				UN_LOGF(LogFlecsComponentProperties, Log,
					"Updated component properties: %s", *FString(Name.data()));
			}

			for (const FInstancedStruct& ComponentPropertyStruct : ComponentPropertyStructs)
			{
				ComponentProperties[Name].ComponentPropertyStructs.Add(ComponentPropertyStruct);
			}
		}

		OnComponentPropertiesRegistered.ExecuteIfBound(ComponentProperties[Name]);
	}
	
	robin_hood::unordered_flat_map<std::string_view, FFlecsComponentProperties> ComponentProperties;

	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
}; // struct FFlecsComponentPropertiesRegistry

#define REGISTER_FLECS_COMPONENT_PROPERTIES(ComponentType, EntitiesArray, ComponentPropertyStructsArray) \
	namespace \
	{ \
		struct FAutoRegister##ComponentType \
		{ \
			FAutoRegister##ComponentType() \
			{ \
			std::vector<flecs::entity_t> Entities = EntitiesArray; \
			TArray<FInstancedStruct> ComponentPropertyStructs = ComponentPropertyStructsArray; \
			FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties(nameof(ComponentType), Entities, ComponentPropertyStructs); \
			} \
		}; \
		static FAutoRegister##ComponentType AutoRegister##ComponentType##_Instance; \
	}
