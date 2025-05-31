// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "SolidMacros/Macros.h"
#include "FlecsTimer.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTimer
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsTimer() = default;
	FORCEINLINE FFlecsTimer(const flecs::timer& InTimer) : Timer(InTimer) {}
	FORCEINLINE FFlecsTimer(const flecs::timer* InTimer) : Timer(*InTimer) {}

	FORCEINLINE void SetTimer(const flecs::timer& InTimer) { Timer = InTimer; }
	FORCEINLINE void SetTimer(const flecs::timer* InTimer) { Timer = *InTimer; }

	NO_DISCARD FORCEINLINE flecs::timer& GetFlecsTimer() { return Timer; }
	NO_DISCARD FORCEINLINE const flecs::timer& GetFlecsTimer() const { return Timer; }

	FORCEINLINE operator flecs::timer&() { return GetFlecsTimer(); }
	FORCEINLINE operator const flecs::timer&() const { return GetFlecsTimer(); }

	NO_DISCARD FORCEINLINE flecs::timer* operator->() { return &Timer; }
	NO_DISCARD FORCEINLINE const flecs::timer* operator->() const { return &Timer; }

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsTimer& Other) const
	{
		return Timer == Other.Timer;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsTimer& Other) const
	{
		return !(*this == Other);
	}

	NO_DISCARD FORCEINLINE bool operator==(const flecs::timer& Other) const
	{
		return Timer == Other;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const flecs::timer& Other) const
	{
		return !(*this == Other);
	}

	NO_DISCARD FORCEINLINE FFlecsEntityHandle GetEntity() const
	{
		return FFlecsEntityHandle(Timer);
	}

	FORCEINLINE FFlecsTimer& SetInterval(const double InInterval)
	{
		Timer.interval(InInterval);
		return *this;
	}

	NO_DISCARD FORCEINLINE double GetInterval()
	{
		return Timer.interval();
	}

	FORCEINLINE FFlecsTimer& SetTimeout(const double InTimeout)
	{
		Timer.timeout(InTimeout);
		return *this;
	}

	NO_DISCARD FORCEINLINE double GetTimeout()
	{
		return Timer.timeout();
	}

	FORCEINLINE FFlecsTimer& SetRate(const int32 InRate,
		const FFlecsEntityHandle& InTickSource = FFlecsEntityHandle::GetNullHandle())
	{
		Timer.rate(InRate, InTickSource);
		return *this;
	}

	FORCEINLINE void StartTimer()
	{
		Timer.start();
	}

	FORCEINLINE void StopTimer()
	{
		Timer.stop();
	}

	flecs::timer Timer;
	
}; // struct FFlecsTimer
