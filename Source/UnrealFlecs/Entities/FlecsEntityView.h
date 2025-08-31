// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Types/SolidNotNull.h"

#include "FlecsEntityHandleTypes.h"

#include "FlecsEntityView.generated.h"

enum class EFlecsComponentAccessorType : uint8
{
	
}; // enum class EFlecsComponentAccessorType

namespace Unreal::Flecs
{
	
} // namespace Unreal::Flecs

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityView : public FFlecsCommonHandle
{
	GENERATED_BODY()

	using FSelfType = FFlecsEntityView;
	
	static NO_DISCARD SOLID_INLINE FFlecsEntityView GetNullHandle()
	{
		return FFlecsEntityView(flecs::entity::null());
	}

	static NO_DISCARD FFlecsEntityView GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	SOLID_INLINE FFlecsEntityView() = default;
	
	SOLID_INLINE FFlecsEntityView(const flecs::entity& InEntity) : FFlecsCommonHandle(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const flecs::entity& InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world_t* InWorld, const flecs::entity_t InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsEntityView(const FFlecsId InEntity) : FFlecsCommonHandle(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const FFlecsId InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}
	
private:
	flecs::entity_view GetEntityView() const
	{
		return GetEntity().view();
	}
	
}; // struct FFlecsEntityView
