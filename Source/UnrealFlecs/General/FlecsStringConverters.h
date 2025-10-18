// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define FLECS_DEFAULT_STRING_CONVERSION_SIZE DEFAULT_STRING_CONVERSION_SIZE

namespace Unreal::Flecs
{
	template <int32 ConversionSize = FLECS_DEFAULT_STRING_CONVERSION_SIZE>
	NO_DISCARD FORCEINLINE const char* ToCString(const TCHAR* InString) UE_LIFETIMEBOUND
	{
		return StringCast<char, ConversionSize>(InString).Get();
	}

	template <int32 ConversionSize = FLECS_DEFAULT_STRING_CONVERSION_SIZE>
	NO_DISCARD FORCEINLINE const char* ToCString(const FString& InString) UE_LIFETIMEBOUND
	{
		return ToCString<ConversionSize>(*InString);
	}
	
} // namespace Unreal::Flecs
