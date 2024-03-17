// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsSystem.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSystem
{
	GENERATED_BODY()

public:
	FFlecsSystem() = default;
	
	FORCEINLINE FFlecsSystem(const flecs::system& InSystem) : System(InSystem) {}
	FORCEINLINE FFlecsSystem(const flecs::system* InSystem) : System(*InSystem) {}
	
	
	template <typename ...TComponents>
	FORCEINLINE NO_DISCARD static flecs::system_builder<TComponents...> CreateSystem(flecs::world& InWorld, const TCHAR* InName)
	{
		return InWorld.system<TComponents...>(TCHAR_TO_ANSI(InName));
	}

	FORCEINLINE void SetSystem(const flecs::system& InSystem)
	{
		System = InSystem;
	}

	FORCEINLINE NO_DISCARD flecs::system& GetFlecsSystem()
	{
		return System;
	}

	FORCEINLINE NO_DISCARD const flecs::system& GetFlecsSystem() const
	{
		return System;
	}

	FORCEINLINE NO_DISCARD FString GetName() const
	{
		return static_cast<FString>(System.name());
	}


private:
	flecs::system System;
}; // struct FFlecsSystem
