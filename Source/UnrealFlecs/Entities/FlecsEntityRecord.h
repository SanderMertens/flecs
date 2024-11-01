// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsEntityRecord.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2,
	Pair = 3
}; // enum class EFlecsComponentNodeType

UENUM(BlueprintType)
enum class EFlecsPairNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2
}; // enum class EFlecsPairNodeType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPairSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsPairNodeType NodeType = EFlecsPairNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsPairNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsPairNodeType::EntityHandle", EditConditionHides))
	FFlecsEntityHandle EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsPairNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsPairSlot& Other) const
	{
		switch (NodeType)
		{
		case EFlecsPairNodeType::ScriptStruct:
			return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct;
		case EFlecsPairNodeType::EntityHandle:
			return NodeType == Other.NodeType && EntityHandle == Other.EntityHandle;
		case EFlecsPairNodeType::FGameplayTag:
			return NodeType == Other.NodeType && GameplayTag == Other.GameplayTag;
		default: UNLIKELY_ATTRIBUTE
			solid_checkf(false, TEXT("Invalid NodeType"));
			return false;
		}
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsPairSlot& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsPairSlot

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsPairSlot First;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsPairSlot Second;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsPair& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsPair& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void AddToEntity(const FFlecsEntityHandle& InEntityHandle) const
	{
		FFlecsEntityHandle InFirstScriptStructEntity;
		switch (First.NodeType)
		{
		case EFlecsPairNodeType::ScriptStruct:
			switch (Second.NodeType)
			{
			case EFlecsPairNodeType::ScriptStruct:
				InFirstScriptStructEntity
					= InEntityHandle.ObtainComponentTypeStruct(First.ScriptStruct.GetScriptStruct());
				
				if (InFirstScriptStructEntity.Has(flecs::PairIsTag))
				{
					InEntityHandle.SetPairSecond(First.ScriptStruct.GetScriptStruct(),
						Second.ScriptStruct.GetScriptStruct(), Second.ScriptStruct.GetMemory());
				}
				else
				{
					InEntityHandle.SetPair(First.ScriptStruct.GetScriptStruct(),
						First.ScriptStruct.GetMemory(), Second.ScriptStruct.GetScriptStruct());
				}
				
				break;
			case EFlecsPairNodeType::EntityHandle:
				InEntityHandle.SetPair(First.ScriptStruct.GetScriptStruct(), First.ScriptStruct.GetMemory(), Second.EntityHandle);
				break;
			case EFlecsPairNodeType::FGameplayTag:
				InEntityHandle.SetPair(First.ScriptStruct.GetScriptStruct(), First.ScriptStruct.GetMemory(), Second.GameplayTag);
				break;
			default: UNLIKELY_ATTRIBUTE
				solid_checkf(false, TEXT("Invalid Second NodeType"));
				break;
			}
			break;
		case EFlecsPairNodeType::EntityHandle:
			switch (Second.NodeType)
			{
			case EFlecsPairNodeType::ScriptStruct:
				InEntityHandle.SetPairSecond(First.EntityHandle,
					Second.ScriptStruct.GetScriptStruct(), Second.ScriptStruct.GetMemory());
				break;
			case EFlecsPairNodeType::EntityHandle:
				InEntityHandle.AddPair(First.EntityHandle, Second.EntityHandle);
				break;
			case EFlecsPairNodeType::FGameplayTag:
				InEntityHandle.AddPair(First.EntityHandle, Second.GameplayTag);
				break;
			default: UNLIKELY_ATTRIBUTE
				solid_checkf(false, TEXT("Invalid Second NodeType"));
				break;
			}
			break;
		case EFlecsPairNodeType::FGameplayTag:
			switch (Second.NodeType)
			{
			case EFlecsPairNodeType::ScriptStruct:
				InEntityHandle.SetPairSecond(First.GameplayTag,
					Second.ScriptStruct.GetScriptStruct(), Second.ScriptStruct.GetMemory());
				break;
			case EFlecsPairNodeType::EntityHandle:
				InEntityHandle.AddPair(First.GameplayTag, Second.EntityHandle);
				break;
			case EFlecsPairNodeType::FGameplayTag:
				InEntityHandle.AddPair(First.GameplayTag, Second.GameplayTag);
				break;
			default: UNLIKELY_ATTRIBUTE
				solid_checkf(false, TEXT("Invalid Second NodeType"));
				break;
			}
			break;
		default: UNLIKELY_ATTRIBUTE
			solid_checkf(false, TEXT("Invalid First NodeType"));
			break;
		}
	}
	
}; // struct FFlecsPair

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::Pair", EditConditionHides))
	FFlecsPair Pair;
	
	FORCEINLINE NO_DISCARD bool operator==(const FFlecsTraitTypeInfo& Other) const
	{
		switch (NodeType)
		{
		case EFlecsComponentNodeType::ScriptStruct:
			return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct;
		case EFlecsComponentNodeType::EntityHandle:
			return NodeType == Other.NodeType && EntityHandle == Other.EntityHandle;
		case EFlecsComponentNodeType::FGameplayTag:
			return NodeType == Other.NodeType && GameplayTag == Other.GameplayTag;
		case EFlecsComponentNodeType::Pair:
			return NodeType == Other.NodeType && Pair == Other.Pair;
		default: UNLIKELY_ATTRIBUTE
			solid_checkf(false, TEXT("Invalid NodeType"));
			return false;
		}
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
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::Pair", EditConditionHides))
	FFlecsPair Pair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptStruct", EditConditionHides))
	TArray<FFlecsTraitTypeInfo> Traits;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsComponentTypeInfo& Other) const
	{
		switch (NodeType)
		{
		case EFlecsComponentNodeType::ScriptStruct:
			return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct && Traits == Other.Traits;
		case EFlecsComponentNodeType::EntityHandle:
			return NodeType == Other.NodeType && EntityHandle == Other.EntityHandle;
		case EFlecsComponentNodeType::FGameplayTag:
			return NodeType == Other.NodeType && GameplayTag == Other.GameplayTag;
		default: UNLIKELY_ATTRIBUTE
			solid_checkf(false, TEXT("Invalid NodeType"));
			return false;
		}
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

	FORCEINLINE FFlecsEntityRecord() = default;

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

		for (const auto& [NodeType, ScriptStruct, EntityHandle, GameplayTag, Pair, Traits] : Components)
		{
			switch (NodeType)
			{
			case EFlecsComponentNodeType::ScriptStruct:
				
				InEntityHandle.Set(ScriptStruct);

				for (const auto& [TraitNodeType, TraitScriptStruct, TraitEntityHandle, TraitGameplayTag, TraitPair] : Traits)
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
					case EFlecsComponentNodeType::Pair:
						TraitPair.AddToEntity(InEntityHandle);
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
			case EFlecsComponentNodeType::Pair:
				Pair.AddToEntity(InEntityHandle);
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
