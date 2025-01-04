// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsQueryInOut.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryInOut : uint8
{
	Default = flecs::InOutDefault,
	None = flecs::InOutNone,
	Read = flecs::In,
	Write = flecs::Out,
	ReadWrite = flecs::InOut UMETA(DisplayName = "Read/Write"),
	Filter = flecs::InOutFilter /**< Same as None + prevents term from triggering observers */
}; // enum class EFlecsQueryInOut

FORCEINLINE NO_DISCARD constexpr flecs::inout_kind_t ToFlecsInOut(EFlecsQueryInOut InOut) NOEXCEPT
{
	return static_cast<flecs::inout_kind_t>(InOut);
}