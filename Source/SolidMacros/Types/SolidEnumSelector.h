// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"
#include "SolidMacros/Concepts/SolidConcepts.h"

#include "SolidEnumSelector.generated.h"

/**
 * Taken from @FEnumSelector in the PCG Module.
 */
USTRUCT(BlueprintType)
struct SOLIDMACROS_API FSolidEnumSelector
{
	GENERATED_BODY()

public:
	FSolidEnumSelector() = default;

	// @TODO: fix this not having anough constraints
	template <typename TEnum>
	requires (std::is_enum<TEnum>::value)
	static NO_DISCARD FSolidEnumSelector Make(const TEnum InValue)
	{
		//using EnumType = decltype(InValue);
		//static_assert(Solid::TStaticEnumConcept<EnumType>, "FSolidEnumSelector::Make<TEnum>: TEnum must be a UENUM type.");
		
		return FSolidEnumSelector(StaticEnum<TEnum>(), static_cast<int64>(InValue));
	}

	static NO_DISCARD FSolidEnumSelector Make(UEnum* InClass, const int64 InValue)
	{
		return FSolidEnumSelector(InClass, InValue);
	}

	FORCEINLINE FSolidEnumSelector(UEnum* InClass, const int64 InValue)
		: Class(InClass), Value(InValue)
	{
	}
	
	UPROPERTY(DisplayName="Enum Class")
	TObjectPtr<UEnum> Class;

	UPROPERTY(DisplayName="Enum Value")
	int64 Value = 0;

	FText GetDisplayName() const;
	FString GetCultureInvariantDisplayName() const;
	
}; // struct FSolidEnumSelector
