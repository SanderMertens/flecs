// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Properties/FlecsComponentProperties.h"

#include "FlecsCollectionTypes.generated.h"

class UFlecsCollectionDataAsset;

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionPrefabTag
{
	GENERATED_BODY()
}; // struct FFlecsCollectionPrefabTag

REGISTER_FLECS_COMPONENT(FFlecsCollectionPrefabTag,
[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::Prefab, flecs::DontInherit);
	});

/** compose another Collection by reference (adds (IsA, @param Collection) in compile-time and then removes itself). */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionReferenceComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Flecs")
	TObjectPtr<UFlecsCollectionDataAsset> Collection = nullptr;
	
}; // struct FFlecsCollectionReferenceComponent

REGISTER_FLECS_COMPONENT(FFlecsCollectionReferenceComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionSlotTag
{
	GENERATED_BODY()
}; // struct FFlecsCollectionSlotTag

REGISTER_FLECS_COMPONENT(FFlecsCollectionSlotTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

