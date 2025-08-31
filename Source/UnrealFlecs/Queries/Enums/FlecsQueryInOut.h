// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsQueryInOut.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryInOut : uint8
{
	Default = flecs::InOutDefault, /**< InOut for regular terms, In for shared terms */
	None = flecs::InOutNone, /**< Term is neither read nor written */
	Read = flecs::In, /** Term is only read */
	Write = flecs::Out, /**< Term is only written */
	ReadWrite = flecs::InOut UMETA(DisplayName = "Read/Write"), /**< Term is both read and written */
	Filter = flecs::InOutFilter /** Same as None + prevents term from triggering observers */
}; // enum class EFlecsQueryInOut

NO_DISCARD FORCEINLINE constexpr flecs::inout_kind_t ToFlecsInOut(EFlecsQueryInOut InOut)
{
	return static_cast<flecs::inout_kind_t>(InOut);
}