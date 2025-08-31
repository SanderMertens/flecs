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
	FORCEINLINE FFlecsPrimaryAssetComponent(const FSoftObjectPath& InObjectPath)
		: ObjectPath(InObjectPath)
	{
	}

	UPROPERTY()
	FSoftObjectPath ObjectPath;
	
}; // struct FFlecsPrimaryAssetComponent

REGISTER_FLECS_COMPONENT(FFlecsPrimaryAssetComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponent)
	{
		InComponent
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});
