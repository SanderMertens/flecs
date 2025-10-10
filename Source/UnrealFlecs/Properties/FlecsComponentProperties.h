// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"

#include "SolidMacros/Macros.h"
#include "Standard/robin_hood.h"

#include "Entities/FlecsComponentHandle.h"

namespace Unreal::Flecs
{
	using FFlecsComponentFunction = std::function<void(flecs::world, const FFlecsComponentHandle&)>;
} // namespace Unreal::Flecs

struct UNREALFLECS_API FFlecsComponentProperties
{
	std::string Name;
	TWeakObjectPtr<UScriptStruct> Struct;

	uint32 Size = 1;
	uint16 Alignment = 1;

	Unreal::Flecs::FFlecsComponentFunction RegistrationFunction;
}; // struct FFlecsComponentProperties

DECLARE_MULTICAST_DELEGATE_OneParam(FOnComponentPropertiesRegistered, FFlecsComponentProperties);

struct UNREALFLECS_API FFlecsComponentPropertiesRegistry final
{
	static FFlecsComponentPropertiesRegistry Instance;
	
public:
	static NO_DISCARD FFlecsComponentPropertiesRegistry& Get()
	{
		return Instance;
	}

	void RegisterComponentProperties(const std::string& Name,
	                                 UScriptStruct* Struct,
	                                 const uint32 Size, const uint16 Alignment,
	                                 const Unreal::Flecs::FFlecsComponentFunction& RegistrationFunction);

	NO_DISCARD bool ContainsComponentProperties(const std::string_view Name) const;

	NO_DISCARD bool ContainsComponentProperties(const FString& Name) const;

	NO_DISCARD const FFlecsComponentProperties& GetComponentProperties(const std::string_view Name) const;

	NO_DISCARD const FFlecsComponentProperties& GetComponentProperties(const FString& Name) const;

	robin_hood::unordered_map<std::string, FFlecsComponentProperties> ComponentProperties;
	FOnComponentPropertiesRegistered OnComponentPropertiesRegistered;
	
}; // struct FFlecsComponentPropertiesRegistry

// std::function<void(flecs::world, const FFlecsComponentHandle&)>
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