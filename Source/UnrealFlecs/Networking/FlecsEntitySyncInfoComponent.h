// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolidMacros/Macros.h"
#include "FlecsEntitySyncInfoComponent.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentReplicationType : uint8
{
	Name = 0,
	ScriptStruct,
	ScriptClass,
	ScriptObject,
}; // enum class EFlecsComponentReplicationType

USTRUCT(BlueprintType)
struct FFlecsComponentReplicationInfo
{
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Replication Info")
	EFlecsComponentReplicationType ReplicationType = EFlecsComponentReplicationType::Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Replication Info",
		meta = (EditCondition = "ReplicationType == EFlecsComponentReplicationType::Name", EditConditionHides))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Replication Info",
		meta = (EditCondition = "ReplicationType == EFlecsComponentReplicationType::ScriptStruct", EditConditionHides))
	TObjectPtr<UScriptStruct> ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Replication Info",
		meta = (EditCondition = "ReplicationType == EFlecsComponentReplicationType::ScriptClass", EditConditionHides))
	TSubclassOf<UObject> ScriptClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Replication Info",
		meta = (EditCondition = "ReplicationType == EFlecsComponentReplicationType::ScriptObject", EditConditionHides))
	TObjectPtr<UObject> ScriptObject;

	FORCEINLINE NO_DISCARD friend uint32 GetTypeHash(const FFlecsComponentReplicationInfo& InComponent)
	{
		switch (InComponent.ReplicationType)
		{
		case EFlecsComponentReplicationType::Name:
			return HashCombineFast(GetTypeHash(InComponent.Name), GetTypeHash(InComponent.ReplicationType));
		case EFlecsComponentReplicationType::ScriptStruct:
			return HashCombineFast(GetTypeHash(InComponent.ScriptStruct), GetTypeHash(InComponent.ReplicationType));
		case EFlecsComponentReplicationType::ScriptClass:
			return HashCombineFast(GetTypeHash(InComponent.ScriptClass), GetTypeHash(InComponent.ReplicationType));
		case EFlecsComponentReplicationType::ScriptObject:
			return HashCombineFast(GetTypeHash(InComponent.ScriptObject), GetTypeHash(InComponent.ReplicationType));
		default: UNLIKELY_ATTRIBUTE
			checkf(false, TEXT("Invalid replication type: %d"), static_cast<int32>(InComponent.ReplicationType));
			return 0;
		}
	}

	FORCEINLINE bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ReplicationType;

		switch (ReplicationType)
		{
		case EFlecsComponentReplicationType::Name:
			Ar << Name;
			break;
		case EFlecsComponentReplicationType::ScriptStruct:
			Ar << ScriptStruct;
			break;
		case EFlecsComponentReplicationType::ScriptClass:
			Ar << ScriptClass;
			break;
		case EFlecsComponentReplicationType::ScriptObject:
			Ar << ScriptObject;
			break;
		default: UNLIKELY_ATTRIBUTE
			checkf(false, TEXT("Invalid replication type: %d"), static_cast<int32>(ReplicationType));
			break;
		}

		bOutSuccess = true;
		return true;
	}
	
}; // struct FFlecsComponentReplicationInfo

template<>
struct TStructOpsTypeTraits<FFlecsComponentReplicationInfo> : public TStructOpsTypeTraitsBase2<FFlecsComponentReplicationInfo>
{
	enum
	{
		WithNetSerializer = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsComponentReplicationInfo>

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntitySyncInfoComponent final
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint32 bInitialized : 1 = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component")
	uint32 bAlwaysRelevant : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component")
	uint32 bReplicateAllComponents : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component",
		meta = (EditCondition = "bReplicateAllComponents", EditConditionHides))
	TSet<FFlecsComponentReplicationInfo> ComponentsToReplicate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Sync Info Component")
	TSet<FFlecsComponentReplicationInfo> ComponentsToNotReplicate;
	
}; // struct FFlecsEntitySyncInfoComponent
