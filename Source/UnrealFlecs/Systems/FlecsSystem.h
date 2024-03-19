// Solstice Games © 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsEntityHandle.h"
#include "Queries/FlecsQuery.h"
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

	FORCEINLINE void SetContext(void* InContext)
	{
		System.ctx(InContext);
	}

	FORCEINLINE NO_DISCARD void* GetContext() const
	{
		return System.ctx();
	}

	FORCEINLINE NO_DISCARD FFlecsQuery GetQuery() const
	{
		return System.query();
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetEntity() const
	{
		return FFlecsEntityHandle(System);
	}

	FORCEINLINE void SetInterval(const float InInterval)
	{
		System.interval(InInterval);
	}

	FORCEINLINE NO_DISCARD float GetInterval()
	{
		return System.interval();
	}

	FORCEINLINE void SetTimeout(const float InTimeout)
	{
		System.timeout(InTimeout);
	}

	FORCEINLINE NO_DISCARD float GetTimeout()
	{
		return System.timeout();
	}

	FORCEINLINE void SetRate(const float InRate)
	{
		System.rate(InRate);
	}

	FORCEINLINE void StartTimer()
	{
		System.start();
	}

	FORCEINLINE void StopTimer()
	{
		System.stop();
	}
	
	flecs::system System;
}; // struct FFlecsSystem
