// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsPrimaryAssetComponent.generated.h"

USTRUCT()
struct UNREALFLECS_API FFlecsPrimaryAssetComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsPrimaryAssetComponent() = default;
	FORCEINLINE FFlecsPrimaryAssetComponent(const FPrimaryAssetId& InPrimaryAssetId) : PrimaryAssetId(InPrimaryAssetId) {}

	FORCEINLINE NO_DISCARD FPrimaryAssetId GetPrimaryAssetId() const
	{
		return PrimaryAssetId;
	}

	FORCEINLINE void SetPrimaryAssetId(const FPrimaryAssetId& InPrimaryAssetId)
	{
		PrimaryAssetId = InPrimaryAssetId;
	}

	FORCEINLINE NO_DISCARD bool IsValid() const
	{
		return PrimaryAssetId.IsValid();
	}

	UPROPERTY()
	FPrimaryAssetId PrimaryAssetId;
	
}; // struct FFlecsPrimaryAssetComponent

REGISTER_FLECS_COMPONENT(FFlecsPrimaryAssetComponent, [](flecs::world InWorld, flecs::untyped_component InComponent)
{
	InComponent.add(flecs::OnInstantiate, flecs::DontInherit);
});
