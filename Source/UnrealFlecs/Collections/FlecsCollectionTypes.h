// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Properties/FlecsComponentProperties.h"
#include "FlecsCollectionId.h"

#include "FlecsCollectionTypes.generated.h"

class UFlecsCollectionDataAsset;
class UFlecsCollectionClass;

UENUM(BlueprintType)
enum class EFlecsCollectionReferenceMode : uint8
{
	Asset = 0,
	Id = 1,
	UClass = 2,
	
}; // enum class EFlecsCollectionReferenceMode

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionReference
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionReference() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	EFlecsCollectionReferenceMode Mode = EFlecsCollectionReferenceMode::Asset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs", meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::Asset", EditConditionHides))
	TObjectPtr<UFlecsCollectionDataAsset> Asset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::UClass", EditConditionHides,
			MustImplement = "/Script/UnrealFlecs.FlecsCollectionInterface"))
	TSubclassOf<UObject> Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::Id", EditConditionHides))
	FFlecsCollectionId Id;
	
}; // struct FFlecsCollectionReference

/** compose another Collection by reference (adds (IsA, @param Collection) in compile-time and then removes itself). */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionReferenceComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionReferenceComponent() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	TArray<FFlecsCollectionReference> Collections;
	
}; // struct FFlecsCollectionReferenceComponent

REGISTER_FLECS_COMPONENT(FFlecsCollectionReferenceComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.Add(flecs::Sparse)
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionPrefabTag
{
	GENERATED_BODY()
}; // struct FFlecsCollectionPrefabTag

REGISTER_FLECS_COMPONENT(FFlecsCollectionPrefabTag,
[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit)
			.AddPair(flecs::With, flecs::Prefab);
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

USTRUCT(BlueprintInternalUseOnly)
struct UNREALFLECS_API FFlecsCollectionStructInterface
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionStructInterface() = default;
	virtual ~FFlecsCollectionStructInterface() = default;

	
	
}; // struct FFlecsCollectionStructInterface

