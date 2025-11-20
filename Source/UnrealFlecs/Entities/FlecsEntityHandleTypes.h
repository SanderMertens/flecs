// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "StructUtils/StructView.h"

#include "Types/SolidEnumSelector.h"
#include "Types/SolidNotNull.h"

#include "FlecsId.h"

class UFlecsWorld;
struct FFlecsEntityHandle;

#ifndef DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION

#define DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(Name) \
	extern Unreal::Flecs::FEntityNetSerializeFunction Name;

#endif // DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION

#ifndef DEFINE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION

#define DEFINE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(Name, Lambda) \
	Unreal::Flecs::FEntityNetSerializeFunction Name = Lambda;

#endif // DEFINE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION

namespace Unreal::Flecs
{
	/** @TODO: Documentation
	 * Global NetSerialize function pointer(also there is an option for a local override using the FFlecs
	 */
	
	using FEntityNetSerializeFunction
		= std::function<bool(FFlecsEntityHandle&, TSolidNotNull<UFlecsWorld*>, FArchive&, UPackageMap*, bool&)>;

	UNREALFLECS_API DECLARE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(EmptyNetSerializeFunction);

	UNREALFLECS_API extern Unreal::Flecs::FEntityNetSerializeFunction* GNetSerializeFunctionPtr;

	/*
	 * Runtime Types that may have data associated with them
	 */
	template <typename T>
	concept TFlecsEntityFunctionInputDataTypeConcept =
		std::is_convertible_v<T, const FFlecsId> ||
		std::is_convertible_v<T, const UScriptStruct*>;

	/*
	 * Only Runtime types allowed are types in which it's impossible to have data, e.g. GameplayTags
	 */
	template <typename T>
	concept TFlecsEntityFunctionInputNoDataTypeConcept =
		std::is_convertible_v<T, const FGameplayTag&>;

	/*
	 * Supports both runtime data and no-data types
	 */
	template <typename T>
	concept TFlecsEntityFunctionInputTypeConcept =
		TFlecsEntityFunctionInputDataTypeConcept<T> || TFlecsEntityFunctionInputNoDataTypeConcept<T>;

	/*
	 * Data types that have associated UEnum types
	 */
	template <typename T>
	concept TFlecsEntityFunctionUEnumTypeConcept = std::is_convertible_v<T, const UEnum*>;

	template <typename T>
	concept TFlecsEntityFunctionValueEnumTypeConcept =
		std::is_convertible_v<T, const FSolidEnumSelector&>;

	/*
	 * Data types that have associated UEnum types (without an Enum Value) or are input types
	 */
	template <typename T>
	concept TFlecsEntityFunctionDataTypeWithEnumNoValueConcept =
		TFlecsEntityFunctionUEnumTypeConcept<T> ||
		TFlecsEntityFunctionInputTypeConcept<T>;

	/*
	 * Runtime UScriptStruct type and Const Data alongside it
	 */
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
