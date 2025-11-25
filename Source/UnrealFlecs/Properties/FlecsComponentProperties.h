// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <string>

#include "flecs.h"

#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"

#include "SolidMacros/Macros.h"
#include "Standard/robin_hood.h"
#include "Types/SolidCppStructOps.h"

#include "Entities/FlecsComponentHandle.h"

namespace Unreal::Flecs
{
	using FFlecsComponentFunction = std::function<void(flecs::world, const FFlecsComponentHandle&)>;

	namespace internal
	{
		template <typename T>
		FORCEINLINE UScriptStruct* GetScriptStructIf()
		{
			if constexpr (Solid::IsScriptStruct<T>())
			{
				return TBaseStructure<T>::Get();
			}
			else
			{
				return nullptr;
			}
		}
		
	} // namespace internal
	
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

#define INTERNAL_REGISTER_FLECS_COMPONENT_IMPL(Name, RegistrationFunction) \
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
						#Name, Unreal::Flecs::internal::GetScriptStructIf<Name>(), \
						sizeof(Name), alignof(Name), RegistrationFunction); \
						\
						if constexpr (std::is_move_constructible<Name>::value || std::is_move_assignable<Name>::value) \
						{ \
							FSolidMoveableStructRegistry::Get().RegisterMovableScriptStruct<Name>(); \
						} \
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

#define INTERNAL_REGISTER_FLECS_COMPONENT_1(Name) \
	INTERNAL_REGISTER_FLECS_COMPONENT_IMPL(Name, Unreal::Flecs::FFlecsComponentFunction{})

#define INTERNAL_REGISTER_FLECS_COMPONENT_2(Name, RegistrationFunction) \
	INTERNAL_REGISTER_FLECS_COMPONENT_IMPL(Name, RegistrationFunction)

// @TODO: Consider adding Auto-Registration
// std::function<void(flecs::world, const FFlecsComponentHandle&)>
#define REGISTER_FLECS_COMPONENT(...) \
	PREPROCESSOR_APPEND_VA_ARG_COUNT(INTERNAL_REGISTER_FLECS_COMPONENT_, ##__VA_ARGS__)(__VA_ARGS__)