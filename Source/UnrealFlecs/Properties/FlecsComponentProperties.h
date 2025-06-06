// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>
#include <vector>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Standard/robin_hood.h"
#include "SolidMacros/Macros.h"
#include "Types/SolidNotNull.h"
#include "Unlog/Unlog.h"

namespace Unreal::Flecs
{
	using FlecsComponentFunction = std::function<void(flecs::world, flecs::untyped_component)>;
} // namespace Unreal::Flecs

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string Name;
	TWeakObjectPtr<UScriptStruct> Struct;

	uint32 Size = 1;
	uint16 Alignment = 1;

	Unreal::Flecs::FlecsComponentFunction RegistrationFunction;
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
	                                             const uint32 Size, const uint16 Alignment,
	                                             const Unreal::Flecs::FlecsComponentFunction& RegistrationFunction)
	{
		UNLOG_CATEGORY_SCOPED(LogFlecsComponentProperties);

		solid_checkf(!Name.empty(), TEXT("Component properties name is empty!"));
		
		ComponentProperties[Name] = FFlecsComponentProperties
		{
			.Name = Name,
			.Struct = Struct,
			.Size = Size, .Alignment = Alignment,
			.RegistrationFunction = RegistrationFunction
		};
		
		OnComponentPropertiesRegistered.ExecuteIfBound(ComponentProperties[Name]);
	}

	NO_DISCARD FORCEINLINE bool ContainsComponentProperties(const std::string& Name) const
	{
		return ComponentProperties.contains(Name);
	}

	NO_DISCARD FORCEINLINE const FFlecsComponentProperties* GetComponentProperties(const std::string& Name) const
	{
		checkf(!Name.empty(), TEXT("Component properties name is empty!"));
		checkf(ComponentProperties.contains(Name), TEXT("Component properties not found!"));
		return &ComponentProperties.at(Name);
	}
	
	robin_hood::unordered_flat_map<std::string, FFlecsComponentProperties> ComponentProperties;
	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
	
}; // struct FFlecsComponentPropertiesRegistry

// std::function<void(flecs::world, flecs::untyped_component)>
#define REGISTER_FLECS_COMPONENT(Name, RegistrationFunction) \
	namespace \
	{ \
		struct FFlecs_AutoRegister_##Name \
		{ \
			FFlecs_AutoRegister_##Name() \
			{ \
				FCoreDelegates::OnPostEngineInit.AddLambda([]() \
				{ \
					if constexpr (Solid::IsScriptStruct<Name>()) \
					{ \
						FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties( \
						#Name, TBaseStructure<Name>::Get(), sizeof(Name), alignof(Name), RegistrationFunction); \
					} \
					else \
					{ \
						FFlecsComponentPropertiesRegistry::Get().RegisterComponentProperties( \
							#Name, nullptr, sizeof(Name), alignof(Name), RegistrationFunction); \
					} \
				}); \
			} \
		}; \
		static FFlecs_AutoRegister_##Name AutoRegister_##Name; \
	} 