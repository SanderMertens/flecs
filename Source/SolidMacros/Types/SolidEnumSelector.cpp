// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "SolidEnumSelector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SolidEnumSelector)

FText FSolidEnumSelector::GetDisplayName() const
{
	return Class ? Class->GetDisplayNameTextByValue(Value) : FText{};
}

FString FSolidEnumSelector::GetCultureInvariantDisplayName() const
{
	return GetDisplayName().BuildSourceString();
}
