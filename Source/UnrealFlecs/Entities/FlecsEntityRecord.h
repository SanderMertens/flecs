// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Collections/FlecsComponentCollectionObject.h"
#include "StructUtils/InstancedStruct.h"
#include "Entities/FlecsEntityHandle.h"
#include "Properties/FlecsComponentProperties.h"
#include "FlecsEntityRecord.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2,
	Pair = 3 /* All Pairs if both are component types then the first type is assumed as value */
}; // enum class EFlecsComponentNodeType

UENUM(BlueprintType)
enum class EFlecsPairNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2
}; // enum class EFlecsPairNodeType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRecordPairSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsPairNodeType PairNodeType = EFlecsPairNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct PairScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::EntityHandle", EditConditionHides))
	FFlecsId EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsRecordPairSlot& Other) const
	{
		switch (PairNodeType)
		{
		case EFlecsPairNodeType::ScriptStruct:
			{
				return PairNodeType == Other.PairNodeType && PairScriptStruct == Other.PairScriptStruct;
			}
		case EFlecsPairNodeType::EntityHandle:
			{
				return PairNodeType == Other.PairNodeType && EntityHandle == Other.EntityHandle;
			}
		case EFlecsPairNodeType::FGameplayTag:
			{
				return PairNodeType == Other.PairNodeType && GameplayTag == Other.GameplayTag;
			}
		}

		UNREACHABLE
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsRecordPairSlot& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsPairSlot

UENUM(BlueprintType)
enum class EFlecsValuePairType : uint8
{
	First = 0,
}; // enum class EFlecsValuePairType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRecordPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsRecordPairSlot First;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsRecordPairSlot Second;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsValuePairType PairValueType = EFlecsValuePairType::First;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsRecordPair& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsRecordPair& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void AddToEntity(const FFlecsEntityHandle& InEntityHandle) const
	{
		switch (First.PairNodeType)
		{
		case EFlecsPairNodeType::ScriptStruct:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							if (PairValueType == EFlecsValuePairType::First)
							{
								InEntityHandle.SetPair(First.PairScriptStruct.GetScriptStruct(),
									First.PairScriptStruct.GetMemory(), Second.PairScriptStruct.GetScriptStruct());
							}
						}
					break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(), Second.EntityHandle);
						}
					break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(), Second.GameplayTag);
						}
					break;
				}
			}
			break;
		case EFlecsPairNodeType::EntityHandle:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.PairScriptStruct.GetScriptStruct());
							InEntityHandle.SetPairSecond(First.EntityHandle, Second.PairScriptStruct.GetScriptStruct(),
								Second.PairScriptStruct.GetMemory());
						}
					break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.EntityHandle);
						}
					break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.GameplayTag);
						}
					break;
				}
			}
			break;
		case EFlecsPairNodeType::FGameplayTag:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.PairScriptStruct.GetScriptStruct());
							InEntityHandle.SetPairSecond(First.GameplayTag, Second.PairScriptStruct.GetScriptStruct(),
								Second.PairScriptStruct.GetMemory());
						}
					break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.EntityHandle);
						}
					break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.GameplayTag);
						}
					break;
				}
			}
			break;
		}
	}
	
}; // struct FFlecsPair

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
	FFlecsId EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::Pair", EditConditionHides))
	FFlecsRecordPair Pair;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsComponentTypeInfo& Other) const
	{
		switch (NodeType)
		{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct && Pair == Other.Pair;
				}
			case EFlecsComponentNodeType::EntityHandle:
				{
					return NodeType == Other.NodeType && EntityHandle == Other.EntityHandle;
				}
			case EFlecsComponentNodeType::FGameplayTag:
				{
					return NodeType == Other.NodeType && GameplayTag == Other.GameplayTag;
				}
			case EFlecsComponentNodeType::Pair:
				{
					return NodeType == Other.NodeType && Pair == Other.Pair;
				}
		}

		UNREACHABLE
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
	TArray<FFlecsComponentTypeInfo> Components;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Flecs | Entity Record")
	TArray<TObjectPtr<UFlecsComponentCollectionObject>> Collections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Entity Record")
	TArray<TInstancedStruct<FFlecsEntityRecord>> SubEntities;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsEntityRecord& Other) const
	{
		return Components == Other.Components;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsEntityRecord& Other) const
	{
		return !(*this == Other);
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE void AddComponent(const T& InComponent)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>(InComponent);
		Components.Add(NewComponent);
	}

	FORCEINLINE void AddComponent(const FFlecsEntityHandle& InEntityHandle)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::EntityHandle;
		NewComponent.EntityHandle = InEntityHandle;
		Components.Add(NewComponent);
	}

	FORCEINLINE void AddComponent(const FGameplayTag& InGameplayTag)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::FGameplayTag;
		NewComponent.GameplayTag = InGameplayTag;
		Components.Add(NewComponent);
	}

	FORCEINLINE void AddComponent(const FFlecsRecordPair& InPair)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::Pair;
		NewComponent.Pair = InPair;
		Components.Add(NewComponent);
	}

	FORCEINLINE int32 AddSubEntity(const FFlecsEntityRecord& InSubEntity)
	{
		return SubEntities.Add(TInstancedStruct<FFlecsEntityRecord>::Make(InSubEntity));
	}

	FORCEINLINE void RemoveSubEntity(const int32 InIndex)
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		SubEntities.RemoveAt(InIndex);
	}

	FORCEINLINE void RemoveAllSubEntities()
	{
		SubEntities.Empty();
	}

	FORCEINLINE NO_DISCARD bool HasSubEntities() const
	{
		return !SubEntities.IsEmpty();
	}

	FORCEINLINE NO_DISCARD int32 GetSubEntityCount() const
	{
		return SubEntities.Num();
	}

	FORCEINLINE const FFlecsEntityRecord& GetSubEntity(const int32 InIndex) const
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		return SubEntities[InIndex].Get<FFlecsEntityRecord>();
	}

	FORCEINLINE FFlecsEntityRecord& GetSubEntity(const int32 InIndex)
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		return SubEntities[InIndex].GetMutable<FFlecsEntityRecord>();
	}

	FORCEINLINE void ApplyRecordToEntity(const FFlecsEntityHandle& InEntityHandle) const
	{
		solid_checkf(InEntityHandle.IsValid(), TEXT("Entity Handle is not valid"));

		for (const auto& [NodeType, ScriptStruct, EntityHandle, GameplayTag, Pair] : Components)
		{
			switch (NodeType)
			{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					InEntityHandle.Set(ScriptStruct);
				}
				break;
			case EFlecsComponentNodeType::EntityHandle:
				{
					InEntityHandle.Add(EntityHandle);
				}
				break;
			case EFlecsComponentNodeType::FGameplayTag:
				{
					InEntityHandle.Add(GameplayTag);	
				}
				break;
			case EFlecsComponentNodeType::Pair:
				{
					Pair.AddToEntity(InEntityHandle);
				}
				break;
			}
		}

		for (UFlecsComponentCollectionObject* Collection : Collections)
		{
			InEntityHandle.AddCollection(Collection);
		}

		for (const TInstancedStruct<FFlecsEntityRecord>& SubEntity : SubEntities)
		{
			const FFlecsEntityRecord& NewEntityRecord = SubEntity.Get();
			FFlecsEntityHandle NewEntityHandle = InEntityHandle.GetEntity().world().entity();
			NewEntityRecord.ApplyRecordToEntity(NewEntityHandle);
			InEntityHandle.Add(NewEntityHandle);
		}
	}
	
}; // struct FFlecsEntityRecord

REGISTER_USTRUCT_FLECS_COMPONENT(FFlecsEntityRecord, [](flecs::world InWorld, flecs::untyped_component InComponent)
	{
		InComponent.add(flecs::OnInstantiate, flecs::DontInherit);
	});
