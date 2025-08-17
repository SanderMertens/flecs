// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Entities/FlecsComponentHandle.h"
#include "Standard/robin_hood.h"
#include "SolidMacros/Macros.h"

namespace Unreal::Flecs
{
	using FFlecsComponentFunction = std::function<void(flecs::world, FFlecsComponentHandle&)>;
} // namespace Unreal::Flecs

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string Name;
	TWeakObjectPtr<UScriptStruct> Struct;

	uint32 Size = 1;
	uint16 Alignment = 1;

	Unreal::Flecs::FFlecsComponentFunction RegistrationFunction;
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
	                                             const Unreal::Flecs::FFlecsComponentFunction& RegistrationFunction)
	{
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

	NO_DISCARD FORCEINLINE bool ContainsComponentProperties(const std::string_view Name) const
	{
		return ComponentProperties.contains(Name.data());
	}

	NO_DISCARD FORCEINLINE bool ContainsComponentProperties(const FString& Name) const
	{
		return ComponentProperties.contains(Unreal::Flecs::ToCString(Name));
	}

	NO_DISCARD FORCEINLINE const FFlecsComponentProperties& GetComponentProperties(const std::string_view Name) const
	{
		checkf(!Name.empty(), TEXT("Component properties name is empty!"));
		checkf(ComponentProperties.contains(Name.data()), TEXT("Component properties not found!"));
		return ComponentProperties.at(Name.data());
	}

	NO_DISCARD FORCEINLINE const FFlecsComponentProperties& GetComponentProperties(const FString& Name) const
	{
		solid_checkf(!Name.IsEmpty(), TEXT("Component properties name is empty!"));
		solid_checkf(ComponentProperties.contains(Unreal::Flecs::ToCString(Name)),
			TEXT("Component properties not found!"));
		return ComponentProperties.at(Unreal::Flecs::ToCString(Name));
	}
	
	robin_hood::unordered_flat_map<std::string, FFlecsComponentProperties> ComponentProperties;
	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
	
}; // struct FFlecsComponentPropertiesRegistry

// std::function<void(flecs::world, FFlecsComponentHandle&)>
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