// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

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
	
	template <typename ...TComponents>
	FORCEINLINE explicit FFlecsSystem(flecs::system_builder<TComponents...>& InBuilder)
	{
		System = InBuilder.build();
	}

	FORCEINLINE FFlecsSystem(const FFlecsEntityHandle& InEntity)
	{
		solid_checkf(InEntity.IsValid(), TEXT("Entity is invalid!"));
		System = InEntity.GetEntity().world().system(InEntity.GetEntity());
	}

	template <typename ...TComponents>
	FORCEINLINE flecs::system_builder<TComponents...> ToBuilder() const
	{
		return flecs::system_builder<TComponents...>(GetEntity().GetEntity().world(), System.name().c_str());
	}
	
	template <typename ...TComponents>
	static NO_DISCARD FORCEINLINE flecs::system_builder<TComponents...> CreateSystem(
		flecs::world& InWorld, const TCHAR* InName)
	{
		return InWorld.system<TComponents...>(StringCast<char>(InName).Get());
	}
	
	FORCEINLINE FFlecsSystem& operator=(const flecs::system& InSystem)
	{
		System = InSystem;
		return *this;
	}

	FORCEINLINE FFlecsSystem& operator=(const FFlecsEntityHandle& InEntity)
	{
		solid_checkf(InEntity.IsValid(), TEXT("Entity is invalid!"));
		System = InEntity.GetEntity().world().system(InEntity.GetEntity());
		return *this;
	}

	FORCEINLINE FFlecsSystem& operator=(const flecs::entity& InEntity)
	{
		System = InEntity.world().system(InEntity);
		return *this;
	}

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsSystem& Other) const
	{
		return System == Other.System;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsSystem& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void Enable()
	{
		System.enable();
	}

	FORCEINLINE void Disable()
	{
		System.disable();
	}

	NO_DISCARD FORCEINLINE bool IsEnabled() const
	{
		return !System.has(flecs::Disabled);
	}

	NO_DISCARD FORCEINLINE bool IsDisabled() const
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

	NO_DISCARD FORCEINLINE flecs::system& GetFlecsSystem()
	{
		return System;
	}

	NO_DISCARD FORCEINLINE const flecs::system& GetFlecsSystem() const
	{
		return System;
	}

	NO_DISCARD FORCEINLINE FString GetName() const
	{
		return FString(System.name());
	}

	FORCEINLINE void SetContext(void* InContext)
	{
		System.ctx(InContext);
	}

	NO_DISCARD FORCEINLINE void* GetContext() const
	{
		return System.ctx();
	}

	NO_DISCARD FORCEINLINE FFlecsQuery GetQuery() const
	{
		return System.query();
	}

	NO_DISCARD FORCEINLINE FFlecsEntityHandle GetEntity() const
	{
		return flecs::entity(System);
	}

	FORCEINLINE void SetInterval(const float InInterval)
	{
		System.interval(InInterval);
	}

	NO_DISCARD FORCEINLINE double GetInterval()
	{
		return System.interval();
	}

	FORCEINLINE void SetTimeout(const float InTimeout)
	{
		System.timeout(InTimeout);
	}

	NO_DISCARD FORCEINLINE double GetTimeout()
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

	FORCEINLINE void SetTickSource(const FFlecsEntityHandle& InSource)
	{
		System.set_tick_source(InSource);
	}

	NO_DISCARD FORCEINLINE bool IsValid() const
	{
		return System.is_valid();
	}

	FORCEINLINE operator bool() const
	{
		return IsValid();
	}
	
	flecs::system System;
	
}; // struct FFlecsSystem

NO_DISCARD FORCEINLINE bool IsValid(const FFlecsSystem& InSystem)
{
	return InSystem.IsValid();
}
