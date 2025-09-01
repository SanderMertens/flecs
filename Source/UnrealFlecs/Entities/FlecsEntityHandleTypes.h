// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "StructUtils/StructView.h"

#include "Types/SolidEnumSelector.h"
#include "Types/SolidNotNull.h"

#include "FlecsId.h"

namespace Unreal::Flecs
{
	template <typename T>
	concept TFlecsEntityFunctionInputDataTypeConcept =
		std::is_convertible_v<T, const FFlecsId> ||
		std::is_convertible_v<T, const UScriptStruct*>;

	template <typename T>
	concept TFlecsEntityFunctionInputNoDataTypeConcept =
		std::is_convertible_v<T, const FGameplayTag&>;
	
	template <typename T>
	concept TFlecsEntityFunctionInputTypeConcept =
		TFlecsEntityFunctionInputDataTypeConcept<T> || TFlecsEntityFunctionInputNoDataTypeConcept<T>;

	template <typename T>
	concept TFlecsEntityFunctionUEnumTypeConcept = std::is_convertible_v<T, const UEnum*>;

	template <typename T>
	concept TFlecsEntityFunctionValueEnumTypeConcept =
		std::is_convertible_v<T, const FSolidEnumSelector&>;

	template <typename T>
	concept TFlecsEntityFunctionDataTypeWithEnumNoValueConcept =
		TFlecsEntityFunctionUEnumTypeConcept<T> ||
		TFlecsEntityFunctionInputTypeConcept<T>;

	template <typename T>
	concept TFlecsEntityFunctionUSTRUCTViewDataTypeConcept
		= std::is_convertible_v<T, FConstStructView>;

	// Must have Valid Constructors or be exactly FFlecsId
	template <typename T>
	concept TFlecsEntityHandleTypeConcept = requires(T)
	{
		std::same_as<std::remove_cvref_t<T>, FFlecsId>
		||
		// Otherwise require all these constructors
		(
			std::constructible_from<T, const flecs::entity&> &&
			std::constructible_from<T, const FFlecsId&> &&
			std::constructible_from<T, const flecs::world&, const FFlecsId&> &&
			std::constructible_from<T, const flecs::world_t*, const FFlecsId&> &&
			std::constructible_from<T, TSolidNotNull<const UFlecsWorld*>, FFlecsId>
		);
		
	}; // concept TFlecsEntityHandleTypeConcept
	
} // namespace Unreal::Flecs
