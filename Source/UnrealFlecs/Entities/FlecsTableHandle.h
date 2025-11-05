// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"

#include "SolidMacros/SolidMacros.h"

#include "FlecsTableHandle.generated.h"

USTRUCT()
struct UNREALFLECS_API FFlecsTableHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsTableHandle() = default;

	FORCEINLINE FFlecsTableHandle(const flecs::table& InTable)
		: Table(InTable)
	{
	}

	FORCEINLINE FFlecsTableHandle(flecs::world_t* InWorld, flecs::table_t* InTable)
		: Table(InWorld, InTable)
	{
	}

	FORCEINLINE operator flecs::table&()
	{
		return Table;
	}

	FORCEINLINE operator const flecs::table&() const
	{
		return Table;
	}

	NO_DISCARD FORCEINLINE flecs::table& GetTable()
	{
		return Table;
	}

	NO_DISCARD FORCEINLINE const flecs::table& GetTable() const
	{
		return Table;
	}

	flecs::table Table;
	
}; // struct FFlecsTableHandle