// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Entities/FlecsEntityHandle.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsEntityRecord.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2,
}; // enum class EFlecsComponentNodeType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsTraitTypeInfo final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsComponentNodeType NodeType = EFlecsComponentNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::EntityHandle", EditConditionHides))
	FFlecsEntityHandle EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsTraitTypeInfo& Other) const
	{
		return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct
			&& EntityHandle == Other.EntityHandle && GameplayTag == Other.GameplayTag;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsTraitTypeInfo& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsTraitTypeInfo

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsComponentTypeInfo final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsComponentNodeType NodeType = EFlecsComponentNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::EntityHandle", EditConditionHides))
	FFlecsEntityHandle EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptStruct", EditConditionHides))
	TArray<FFlecsTraitTypeInfo> Traits;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsComponentTypeInfo& Other) const
	{
		return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct
			&& EntityHandle == Other.EntityHandle && GameplayTag == Other.GameplayTag
			&& Traits == Other.Traits;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsComponentTypeInfo& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsComponentTypeInfo

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Record")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Record")
	TArray<FFlecsComponentTypeInfo> Components;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Record")
	TArray<TInstancedStruct<FFlecsEntityRecord>> SubEntities;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsEntityRecord& Other) const
	{
		return Name == Other.Name && Components == Other.Components;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsEntityRecord& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE NO_DISCARD bool IsValid() const
	{
		return !Name.IsEmpty() || !Components.IsEmpty();
	}

	FORCEINLINE void ApplyRecordToEntity(const FFlecsEntityHandle& InEntityHandle) const
	{
		solid_checkf(IsValid(), TEXT("Entity Record is not valid"));
		
		if (!Name.IsEmpty())
		{
			InEntityHandle.SetName(Name);
		}

		for (const auto& [NodeType, ScriptStruct, EntityHandle, GameplayTag, Traits] : Components)
		{
			switch (NodeType)
			{
			case EFlecsComponentNodeType::ScriptStruct:
				
				InEntityHandle.Set(ScriptStruct);

				for (const auto& [TraitNodeType, TraitScriptStruct, TraitEntityHandle, TraitGameplayTag] : Traits)
				{
					switch (TraitNodeType)
					{
					case EFlecsComponentNodeType::ScriptStruct:
						InEntityHandle.SetTrait(ScriptStruct.GetScriptStruct(), TraitScriptStruct);
						break;
					case EFlecsComponentNodeType::EntityHandle:
						InEntityHandle.AddTrait(ScriptStruct.GetScriptStruct(), TraitEntityHandle);
						break;
					case EFlecsComponentNodeType::FGameplayTag:
						InEntityHandle.AddTrait(ScriptStruct.GetScriptStruct(), TraitGameplayTag);
						break;
					default: UNLIKELY_ATTRIBUTE
						solid_checkf(false, TEXT("Invalid TraitNodeType"));
						break;
					}
				}
				
				break;
			case EFlecsComponentNodeType::EntityHandle:
				InEntityHandle.Add(EntityHandle);
				break;
			case EFlecsComponentNodeType::FGameplayTag:
				InEntityHandle.Add(GameplayTag);
				break;
			default: UNLIKELY_ATTRIBUTE
				solid_checkf(false, TEXT("Invalid NodeType"));
				break;
			}
		}

		for (const TInstancedStruct<FFlecsEntityRecord>& SubEntity : SubEntities)
		{
			FFlecsEntityRecord NewEntityRecord = SubEntity.Get();
			FFlecsEntityHandle NewEntityHandle = InEntityHandle.GetEntity().world().entity(StringCast<char>(*NewEntityRecord.Name).Get());
			NewEntityRecord.ApplyRecordToEntity(NewEntityHandle);
			InEntityHandle.Add(NewEntityHandle);
		}
	}

}; // struct FFlecsEntityRecord

REGISTER_FLECS_COMPONENT_PROPERTIES(FFlecsEntityRecord, { flecs::Sparse }, {} );
