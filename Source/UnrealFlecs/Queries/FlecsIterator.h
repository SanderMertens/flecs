// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "SolidMacros/Macros.h"
#include "FlecsIterator.generated.h"

USTRUCT(BlueprintType)
struct FFlecsIterator
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsIterator()
		: Iterator(nullptr)
	{
	}

	FORCEINLINE FFlecsIterator(const flecs::iter& InIterator)
		: Iterator(InIterator)
	{
	}

	FORCEINLINE FFlecsIterator(const flecs::iter* InIterator)
		: Iterator(*InIterator)
	{
	}

	FORCEINLINE operator flecs::iter&() NOEXCEPT
	{
		return Iterator;
	}

	FORCEINLINE operator const flecs::iter&() const NOEXCEPT
	{
		return Iterator;
	}

	FORCEINLINE NO_DISCARD bool HasChanged() const NOEXCEPT
	{
		return const_cast<flecs::iter&>(Iterator).changed();
	}

	FORCEINLINE NO_DISCARD bool HasNext() const NOEXCEPT
	{
		return const_cast<flecs::iter&>(Iterator).next();
	}

	FORCEINLINE NO_DISCARD auto Begin() const NOEXCEPT
	{
		return Iterator.begin();
	}

	FORCEINLINE NO_DISCARD auto End() const NOEXCEPT
	{
		return Iterator.end();
	}

	FORCEINLINE NO_DISCARD auto First() const NOEXCEPT
	{
		return Iterator.begin();
	}

	FORCEINLINE NO_DISCARD auto Last() const NOEXCEPT
	{
		return Iterator.end();
	}
	
	FORCEINLINE NO_DISCARD int32 GetCount() const NOEXCEPT
	{
		return Iterator.count();
	}
	
	flecs::iter Iterator;
}; // struct FFlecsIterator
