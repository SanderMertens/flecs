// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>
#include <vector>

#include "CoreMinimal.h"
#include "flecs.h"
#include "Standard/robin_hood.h"
#include "SolidMacros/Macros.h"
#include "StructUtils/SharedStruct.h"
#include "Unlog/Unlog.h"

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string Name;
	std::vector<flecs::entity_t> Entities;
	
	TArray<FSharedStruct> ComponentPropertyStructs;
}; // struct FFlecsComponentProperties

DECLARE_DELEGATE_OneParam(FOnComponentPropertiesRegistered, FFlecsComponentProperties);

struct UNREALFLECS_API FFlecsComponentPropertiesRegistry final
{
	static FFlecsComponentPropertiesRegistry Instance;
public:
	static FFlecsComponentPropertiesRegistry& Get()
	{
		return Instance;
	}

	FORCEINLINE void RegisterComponentProperties(const std::string& Name, const std::vector<flecs::entity_t>& Entities,
		const TArray<FSharedStruct>& ComponentPropertyStructs, const bool bResetExisting = false, const bool bOverride = false)
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
			ComponentProperties[Name] = FFlecsComponentProperties {
				.Name = Name, .Entities = Entities,
				.ComponentPropertyStructs = ComponentPropertyStructs
			};
			
			UN_LOGF(LogFlecsComponentProperties,
				Log,
				"Registered component properties: %s",
				*FString(Name.data()));
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

			for (const FSharedStruct& ComponentPropertyStruct : ComponentPropertyStructs)
			{
				if (!bOverride)
				{
					if (ComponentProperties[Name].ComponentPropertyStructs.Contains(ComponentPropertyStruct))
					{
						continue;
					}
				}

				ComponentProperties[Name].ComponentPropertyStructs.Add(ComponentPropertyStruct);
			}
		}

		OnComponentPropertiesRegistered.ExecuteIfBound(ComponentProperties[Name]);
	}

	FORCEINLINE NO_DISCARD bool ContainsComponentProperties(const std::string& Name) const
	{
		return ComponentProperties.contains(Name);
	}

	FORCEINLINE const FFlecsComponentProperties* GetComponentProperties(const std::string& Name) const
	{
		checkf(!Name.empty(), TEXT("Component properties name is empty!"));
		checkf(ComponentProperties.contains(Name), TEXT("Component properties not found!"));
		return &ComponentProperties.at(Name);
	}

	FORCEINLINE static void RegisterStructMetaData(UScriptStruct* ScriptStruct, const FString& Tags = FString())
	{
		#if WITH_METADATA
		
		if UNLIKELY_IF(!ensureAlways(IsValid(ScriptStruct)))
		{
			return;
		}

		const FName MetaDataKey = "FlecsTags";

		if (ScriptStruct->HasMetaData(MetaDataKey))
		{
			const FString ExistingTags = ScriptStruct->GetMetaData(MetaDataKey);
			ScriptStruct->SetMetaData(MetaDataKey, *FString::Printf(TEXT("%s %s"), *ExistingTags, *Tags));
		}
		else
		{
			ScriptStruct->SetMetaData(MetaDataKey, *Tags);
		}

		#endif // WITH_METADATA
	}
	
	robin_hood::unordered_flat_map<std::string, FFlecsComponentProperties> ComponentProperties;

	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
}; // struct FFlecsComponentPropertiesRegistry

/**
 * Do not use this macro directly, use REGISTER_COMPONENT_TAG_PROPERTIES or REGISTER_COMPONENT_TRAIT_PROPERTIES
 */
#define PRIVATE_REGISTER_FLECS_PROPERTIES_TAGS_IMPL_(ComponentType, ...) \
	namespace \
	{ \
		struct FAutoRegister##ComponentType##_Tags \
		{ \
			FAutoRegister##ComponentType##_Tags() \
			{ \
				std::vector<flecs::entity_t> Entities = { __VA_ARGS__ } ; \
				FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties(#ComponentType, Entities, {}); \
				if constexpr (Solid::IsStaticStruct<ComponentType>()) \
				{ \
					FCoreDelegates::OnPostEngineInit.AddLambda([]() \
					{ \
						UScriptStruct* ScriptStruct = TBaseStructure<ComponentType>::Get(); \
						FFlecsComponentPropertiesRegistry::Get().RegisterStructMetaData(ScriptStruct, TEXT(#__VA_ARGS__)); \
					}); \
				} \
			} \
		}; \
		static FAutoRegister##ComponentType##_Tags AutoRegister##ComponentType##_Instance_Tags; \
	}

/**
 * Do not use this macro directly, use REGISTER_COMPONENT_TAG_PROPERTIES or REGISTER_COMPONENT_TRAIT_PROPERTIES
 */
#define PRIVATE_REGISTER_FLECS_PROPERTIES_TRAITS_IMPL_(ComponentType, ...) \
	namespace \
	{ \
		static_assert(Solid::IsStaticStruct<ComponentType>(), \
			"ComponentType must be a UScriptStruct!"); \
		struct FAutoRegister##ComponentType##_Traits \
		{ \
			FAutoRegister##ComponentType##_Traits() \
			{ \
				FCoreDelegates::OnPostEngineInit.AddLambda([]() \
				{ \
					TArray<FSharedStruct> ComponentPropertyStructs = { __VA_ARGS__ }; \
					FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties(#ComponentType, {}, ComponentPropertyStructs); \
				}); \
			} \
		}; \
		static FAutoRegister##ComponentType##_Traits AutoRegister##ComponentType##_Instance_Traits; \
	}

#define REGISTER_COMPONENT_TAG_PROPERTIES(ComponentType, ...) \
	PRIVATE_REGISTER_FLECS_PROPERTIES_TAGS_IMPL_(ComponentType, __VA_ARGS__ )

// @TODO: Only Support ScriptStructs for now
#define REGISTER_COMPONENT_TRAIT_PROPERTIES(ComponentType, ...) \
	PRIVATE_REGISTER_FLECS_PROPERTIES_TRAITS_IMPL_(ComponentType, __VA_ARGS__ )

#define TRAIT_PROPERTY_STRUCT(PropertyStruct, ...) \
	FSharedStruct::Make<PropertyStruct>(__VA_ARGS__)
