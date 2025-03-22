// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityRecord.h"
#include "General/FlecsPrimaryDataAsset.h"
#include "FlecsPrefabAsset.generated.h"

USTRUCT(BlueprintType)
struct FFlecsPrefabSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Prefab")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Prefab")
	FFlecsEntityRecord Record;

	// @TODO: Not Implemented
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Prefab")
	TArray<TInstancedStruct<FFlecsPrefabSlotInfo>> SubSlots;
	
}; // struct FFlecsPrefabSlotInfo

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsPrefabAsset : public UFlecsPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsPrefabSlotInfo Prefab;

	// @TODO: Not Implemented
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	TArray<FFlecsPrefabSlotInfo> Slots;

	UPROPERTY()
	FFlecsEntityHandle PrefabEntityHandle;

	virtual void OnEntityCreated(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World) override final;
	virtual void OnEntityDestroyed(const FFlecsEntityHandle& InEntityHandle, UFlecsWorld* World) override final;


}; // class UFlecsPrefabAsset
