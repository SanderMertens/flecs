// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityRecord.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsPrefabComponent.generated.h"

USTRUCT(BlueprintType)
struct FFlecsPrefabComponent final
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsPrefabComponent() = default;
	
	FORCEINLINE FFlecsPrefabComponent(const FFlecsEntityRecord& InEntityRecord,
		const FFlecsEntityHandle& InPrefabObserverEntity)
		: EntityRecord(InEntityRecord)
		, PrefabObserverEntity(InPrefabObserverEntity)
	{
	}

	FORCEINLINE ~FFlecsPrefabComponent()
	{
	}

	FORCEINLINE FFlecsPrefabComponent(const FFlecsPrefabComponent& Other)
		: EntityRecord(Other.EntityRecord)
	{
	}

	FORCEINLINE FFlecsPrefabComponent(FFlecsPrefabComponent&& Other)
		: EntityRecord(MoveTempIfPossible(Other.EntityRecord))
	{
	}

	FORCEINLINE FFlecsPrefabComponent& operator=(const FFlecsPrefabComponent& Other)
	{
		if LIKELY_IF(this != &Other)
		{
			EntityRecord = Other.EntityRecord;
		}
		
		return *this;
	}

	FORCEINLINE FFlecsPrefabComponent& operator=(FFlecsPrefabComponent&& Other)
	{
		if LIKELY_IF(this != &Other)
		{
			EntityRecord = MoveTempIfPossible(Other.EntityRecord);
		}
		
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsPrefabComponent& Other) const
	{
		return EntityRecord == Other.EntityRecord;
	}

	FORCEINLINE bool operator!=(const FFlecsPrefabComponent& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE NO_DISCARD FFlecsEntityRecord& GetEntityRecord()
	{
		return EntityRecord;
	}

	FORCEINLINE NO_DISCARD FFlecsEntityRecord GetEntityRecord() const
	{
		return EntityRecord;
	}

	FORCEINLINE void SetEntityRecord(const FFlecsEntityRecord& InEntityRecord)
	{
		EntityRecord = InEntityRecord;
	}

	FORCEINLINE void SetEntityRecord(FFlecsEntityRecord&& InEntityRecord)
	{
		EntityRecord = MoveTempIfPossible(InEntityRecord);
	}

	FORCEINLINE operator FFlecsEntityRecord&() { return EntityRecord; }
	FORCEINLINE operator const FFlecsEntityRecord&() const { return EntityRecord; }

	FORCEINLINE NO_DISCARD FFlecsEntityHandle GetObserverEntity() const
	{
		return PrefabObserverEntity;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FFlecsEntityRecord EntityRecord;

	UPROPERTY()
	FFlecsEntityHandle PrefabObserverEntity;
	
}; // struct FFlecsPrefabComponent
