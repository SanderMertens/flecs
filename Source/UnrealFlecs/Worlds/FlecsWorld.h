// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs/addons/cpp/world.hpp"
#include "SolidMacros/Macros.h"
#include "UObject/Object.h"
#include "FlecsWorld.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorld
{
	GENERATED_BODY()

public:
	SOLID_INLINE FFlecsWorld() = default;
	SOLID_INLINE FFlecsWorld(flecs::world& InWorld) : World(&InWorld) {}

	SOLID_INLINE void SetWorld(flecs::world& InWorld) { World = &InWorld; }

	SOLID_INLINE NO_DISCARD flecs::world& GetWorld() { return *World; }
	SOLID_INLINE NO_DISCARD const flecs::world& GetWorld() const { return *World; }

	SOLID_INLINE NO_DISCARD operator flecs::world&() { return GetWorld(); }
	SOLID_INLINE NO_DISCARD operator const flecs::world&() const { return GetWorld(); }

	SOLID_INLINE NO_DISCARD bool IsValid() const { return World != nullptr; }

private:
	flecs::world* World = nullptr;
};
