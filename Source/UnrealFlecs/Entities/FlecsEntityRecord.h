// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	Pair = 3 /* @TODO: All Pairs if both are component types then the first type is assumed as value */
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
			{
				return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct;
			}
		case EFlecsPairNodeType::EntityHandle:
			{
				return NodeType == Other.NodeType && EntityHandle == Other.EntityHandle;
			}
		case EFlecsPairNodeType::FGameplayTag:
			{
				return NodeType == Other.NodeType && GameplayTag == Other.GameplayTag;
			}
		}

		UNREACHABLE
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsPairSlot& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsPairSlot

UENUM(BlueprintType)
enum class EFlecsValuePairType : uint8
{
	First = 0,
	Second = 1, // @TODO: Unsupported for now
}; // enum class EFlecsValuePairType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsPairSlot First;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	FFlecsPairSlot Second;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Component Tree")
	EFlecsValuePairType PairType = EFlecsValuePairType::First;

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
		switch (First.NodeType)
		{
		case EFlecsPairNodeType::ScriptStruct:
			{
				switch (Second.NodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							if (PairType == EFlecsValuePairType::First)
							{
								InEntityHandle.SetPair(First.ScriptStruct.GetScriptStruct(),
									First.ScriptStruct.GetMemory(), Second.ScriptStruct.GetScriptStruct());
							}
							else if (PairType == EFlecsValuePairType::Second)
							{
								InEntityHandle.AddPair(First.ScriptStruct.GetScriptStruct(), Second.ScriptStruct.GetScriptStruct());
							}
						}
					break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.ScriptStruct.GetScriptStruct(), Second.EntityHandle);
						}
					break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.ScriptStruct.GetScriptStruct(), Second.GameplayTag);
						}
					break;
				}
			}
			break;
		case EFlecsPairNodeType::EntityHandle:
			{
				switch (Second.NodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.ScriptStruct.GetScriptStruct());
							InEntityHandle.SetPairSecond(First.EntityHandle, Second.ScriptStruct.GetScriptStruct(),
								Second.ScriptStruct.GetMemory());
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
				switch (Second.NodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.ScriptStruct.GetScriptStruct());
							InEntityHandle.SetPairSecond(First.GameplayTag, Second.ScriptStruct.GetScriptStruct(),
								Second.ScriptStruct.GetMemory());
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
			{
				return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct;
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
				{
					return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct && Traits == Other.Traits
					&& Pair == Other.Pair;
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

	FORCEINLINE void AddComponent(const FFlecsPair& InPair)
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
		return SubEntities[InIndex].Get();
	}

	FORCEINLINE FFlecsEntityRecord& GetSubEntity(const int32 InIndex)
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		return SubEntities[InIndex].GetMutable<FFlecsEntityRecord>();
	}

	FORCEINLINE void ApplyRecordToEntity(const FFlecsEntityHandle& InEntityHandle) const
	{
		solid_checkf(InEntityHandle.IsValid(), TEXT("Entity Handle is not valid"));

		for (const auto& [NodeType, ScriptStruct, EntityHandle, GameplayTag, Pair, Traits] : Components)
		{
			switch (NodeType)
			{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					InEntityHandle.Set(ScriptStruct);

					for (const auto& [TraitNodeType, TraitScriptStruct, TraitEntityHandle, TraitGameplayTag, TraitPair] : Traits)
					{
						switch (TraitNodeType)
						{
							case EFlecsComponentNodeType::ScriptStruct:
								{
									InEntityHandle.SetTrait(ScriptStruct.GetScriptStruct(), TraitScriptStruct);
								}
							break;
							case EFlecsComponentNodeType::EntityHandle:
								{
									InEntityHandle.AddTrait(ScriptStruct.GetScriptStruct(), TraitEntityHandle);
								}
							break;
							case EFlecsComponentNodeType::FGameplayTag:
								{
									InEntityHandle.AddTrait(ScriptStruct.GetScriptStruct(), TraitGameplayTag);
								}
							break;
							case EFlecsComponentNodeType::Pair:
								{
									TraitPair.AddToEntity(InEntityHandle);
								}
							break;
						}
					}
					break;
				}
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

		for (const TInstancedStruct<FFlecsEntityRecord>& SubEntity : SubEntities)
		{
			const FFlecsEntityRecord& NewEntityRecord = SubEntity.Get();
			FFlecsEntityHandle NewEntityHandle = InEntityHandle.GetEntity().world().entity();
			NewEntityRecord.ApplyRecordToEntity(NewEntityHandle);
			InEntityHandle.Add(NewEntityHandle);
		}
	}
	
}; // struct FFlecsEntityRecord

REGISTER_COMPONENT_TAG_PROPERTIES(FFlecsEntityRecord, ecs_pair(flecs::OnInstantiate, flecs::DontInherit));
