// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>
#include <vector>

#include "CoreMinimal.h"
#include "flecs.h"
#include "Standard/robin_hood.h"
#include "SolidMacros/Macros.h"
#include "SolidMacros/Concepts/SolidConcepts.h"
#include "Unlog/Unlog.h"

namespace UnrealFlecs
{
	using FlecsComponentFunctionPtr = std::function<void(flecs::world, flecs::untyped_component)>;
} // namespace UnrealFlecs

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string Name;
	UScriptStruct* Struct = nullptr;

	uint32 Size = 1;
	uint16 Alignment = 1;

	UnrealFlecs::FlecsComponentFunctionPtr RegistrationFunction;
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

	FORCEINLINE void RegisterComponentProperties(const std::string& Name,
		UScriptStruct* Struct,
		const uint32 Size, const uint16 Alignment, const UnrealFlecs::FlecsComponentFunctionPtr& RegistrationFunction)
	{
		UNLOG_CATEGORY_SCOPED(LogFlecsComponentProperties);
		
		ComponentProperties[Name] = FFlecsComponentProperties
		{
			.Name = Name, .Struct = Struct,
			.Size = Size, .Alignment = Alignment,
			.RegistrationFunction = RegistrationFunction
		};
		
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
	
	robin_hood::unordered_flat_map<std::string, FFlecsComponentProperties> ComponentProperties;

	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
}; // struct FFlecsComponentPropertiesRegistry

#define REGISTER_USTRUCT_FLECS_COMPONENT(Name, RegistrationFunction) \
	static_assert(Solid::IsStaticStruct<Name>(), "Name must be a static struct! Use REGISTER_FLECS_COMPONENT instead!"); \
	namespace \
	{ \
		struct FFlecs_AutoRegister_##Name \
		{ \
			FFlecs_AutoRegister_##Name() \
			{ \
				FCoreDelegates::OnPostEngineInit.AddLambda([]() \
				{ \
					FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties( \
					#Name, StaticStruct<Name>(), sizeof(Name), alignof(Name), RegistrationFunction); \
				}); \
			} \
		}; \
		static FFlecs_AutoRegister_##Name AutoRegister_##Name; \
	}

#define REGISTER_FLECS_COMPONENT(Name, RegistrationFunction) \
	static_assert(!Solid::IsStaticStruct<Name>(), "Name must not be a static struct! Use REGISTER_USTRUCT_FLECS_COMPONENT instead!"); \
	namespace \
	{ \
		struct FFlecs_AutoRegister_##Name \
		{ \
			FFlecs_AutoRegister_##Name() \
			{ \
				FCoreDelegates::OnPostEngineInit.AddLambda([]() \
				{ \
					FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties( \
						#Name, nullptr, sizeof(StaticStruct), alignof(StaticStruct), RegistrationFunction); \
				}); \
			} \
		}; \
		static FFlecs_AutoRegister_##Name AutoRegister_##Name; \
	} 