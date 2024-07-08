// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
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

	FORCEINLINE NO_DISCARD flecs::timer& GetFlecsTimer() { return Timer; }
	FORCEINLINE NO_DISCARD const flecs::timer& GetFlecsTimer() const { return Timer; }

	FORCEINLINE operator flecs::timer&() { return GetFlecsTimer(); }
	FORCEINLINE operator const flecs::timer&() const { return GetFlecsTimer(); }

	FORCEINLINE NO_DISCARD flecs::timer* operator->() { return &Timer; }
	FORCEINLINE NO_DISCARD const flecs::timer* operator->() const { return &Timer; }

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsTimer& Other) const
	{
		return Timer == Other.Timer;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsTimer& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE NO_DISCARD bool operator==(const flecs::timer& Other) const
	{
		return Timer == Other;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const flecs::timer& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetEntity() const
	{
		return FFlecsEntityHandle(Timer);
	}

	FORCEINLINE FFlecsTimer& SetInterval(const double InInterval)
	{
		Timer.interval(InInterval);
		return *this;
	}

	FORCEINLINE NO_DISCARD double GetInterval()
	{
		return Timer.interval();
	}

	FORCEINLINE FFlecsTimer& SetTimeout(const double InTimeout)
	{
		Timer.timeout(InTimeout);
		return *this;
	}

	FORCEINLINE NO_DISCARD double GetTimeout()
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
