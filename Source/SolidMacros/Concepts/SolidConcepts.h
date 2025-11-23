// Solstice Games © 2023. All Rights Reserved.

#ifndef SOLID_CONCEPTS_H
#define SOLID_CONCEPTS_H

#include "CoreMinimal.h"

#include "Templates/IsInvocable.h"
#include "Templates/IsUEnumClass.h"

#include "SolidMacros/Macros.h"
#include "Concepts/BaseStructureProvider.h"

namespace Solid
{
	template <typename T>
	NO_DISCARD FORCEINLINE constexpr bool IsStaticStruct()
	{
		return  TModels_V<CStaticStructProvider, T>;
	}
	
	template <typename T>
	concept TStaticStructConcept = requires
	{
		requires IsStaticStruct<T>();
	}; // concept TStaticStructConcept
	
	template <typename T>
	NO_DISCARD FORCEINLINE constexpr bool IsBaseStructure()
	{
		return TModels<CBaseStructureProvider, T>::Value;
	}

	template <typename T>
	concept TBaseStructureConcept = requires
	{
		requires IsBaseStructure<T>();
	}; // concept TBaseStructureConcept
	
	template <typename T>
	concept TVariantStructConcept = requires
	{
		requires TBaseStructureConcept<T>;
		requires TIsUECoreVariant<T>::Value;
	}; // concept TVariantStructConcept

	template <typename T>
	NO_DISCARD FORCEINLINE constexpr bool IsScriptStruct()
	{
		return IsBaseStructure<T>() || IsStaticStruct<T>();
	}

	template <typename T>
	concept TScriptStructConcept = requires
	{
		requires IsScriptStruct<T>();
	}; // concept TScriptStructConcept

	template <typename T>
	NO_DISCARD FORCEINLINE constexpr bool IsStaticClass()
	{
		return TModels<CStaticClassProvider, T>::Value;
	}

	template <typename T>
	concept TStaticClassConcept = requires
	{
		requires IsStaticClass<T>();
	}; // concept TStaticClassConcept

	template <typename TBase, typename TDerived>
	NO_DISCARD FORCEINLINE constexpr bool InheritsFrom()
	{
		return std::is_base_of<TBase, TDerived>::value;
	}

	template <typename TBase, typename TDerived>
	concept TInheritsFromConcept = requires
	{
		requires InheritsFrom<TBase, TDerived>();
	}; // concept TInheritsFromConcept

	template <typename T>
	NO_DISCARD FORCEINLINE constexpr bool IsStaticEnum()
	{
		return TIsUEnumClass<T>::Value;
	}

	template <typename T>
	concept TStaticEnumConcept = requires
	{
		requires IsStaticEnum<T>();
	}; // concept TStaticEnumConcept

	template <typename T>
	concept TClassConcept = requires
	{
		requires std::is_class<T>::value;
	}; // concept TClassConcept

	template <typename T>
	concept TIsOptionalConcept = requires
	{
		requires TIsTOptional_V<T>;
	}; // concept TIsOptionalConcept

	template<typename F, typename ...Args>
	concept TInvocableConcept = TIsInvocable<F, Args...>::Value;
	
} // namespace Solid

#endif // #if SOLID_CONCEPTS_H