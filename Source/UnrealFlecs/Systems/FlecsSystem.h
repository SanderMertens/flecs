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

	FORCEINLINE void Enable()
	{
		System.enable();
	}

	FORCEINLINE void Disable()
	{
		System.disable();
	}

	FORCEINLINE NO_DISCARD bool IsEnabled() const
	{
		return !System.has(flecs::Disabled);
	}

	FORCEINLINE NO_DISCARD bool IsDisabled() const
	{
		return System.has(flecs::Disabled);
	}

	FORCEINLINE void Toggle()
	{
		IsEnabled() ? Disable() : Enable();
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

	FORCEINLINE void SetRate(const int32 InRate)
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

	FORCEINLINE flecs::system_runner_fluent Run(const float DeltaTime = 0.0f, void* InParam = nullptr) const
	{
		return System.run(DeltaTime, InParam);
	}

	FORCEINLINE flecs::system_runner_fluent RunWorker(const int32 CurrentStage, const int32 StageCount,
		const float DeltaTime = 0.0f, void* InParam = nullptr) const
	{
		return System.run_worker(CurrentStage, StageCount, DeltaTime, InParam);
	}

	template <typename T>
	FORCEINLINE void SetTickSource()
	{
		System.set_tick_source<T>();
	}

	FORCEINLINE void SetTickSource(const flecs::entity InSource)
	{
		System.set_tick_source(InSource);
	}
	
	flecs::system System;
}; // struct FFlecsSystem
