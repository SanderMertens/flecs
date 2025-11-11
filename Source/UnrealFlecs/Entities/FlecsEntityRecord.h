// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "StructUtils/InstancedStruct.h"

#include "Concepts/SolidConcepts.h"
#include "Types/SolidEnumSelector.h"

#include "Entities/FlecsEntityHandle.h"
#include "Properties/FlecsComponentProperties.h"

#include "FlecsEntityRecord.generated.h"

UENUM(BlueprintType)
enum class EFlecsComponentNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2,
	Pair = 3, /* All Pairs if both are component types then the first type is assumed as value */
	ScriptEnum = 4,
}; // enum class EFlecsComponentNodeType

UENUM(BlueprintType)
enum class EFlecsPairNodeType : uint8
{
	ScriptStruct = 0,
	EntityHandle = 1,
	FGameplayTag = 2,
	//ScriptEnum = 3,
}; // enum class EFlecsPairNodeType

namespace Unreal::Flecs
{
	template <typename T>
	concept CRecordPairSlotType = std::is_convertible<T, FFlecsId>::value
		|| std::is_convertible<T, FGameplayTag>::value
		|| std::is_convertible<T, FInstancedStruct>::value;
		
	
} // namespace Unreal::Flecs

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRecordPairSlot
{
	GENERATED_BODY()

	static NO_DISCARD FFlecsRecordPairSlot Make(const FInstancedStruct& InStruct)
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		OutSlot.PairScriptStruct = InStruct;
		return OutSlot;
	}

	static NO_DISCARD FFlecsRecordPairSlot Make(const TSolidNotNull<const UScriptStruct*> InStructType)
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::ScriptStruct;

		FInstancedStruct NewInstancedStruct;
		NewInstancedStruct.InitializeAs(InStructType);
		
		OutSlot.PairScriptStruct = MoveTemp(NewInstancedStruct);
		return OutSlot;
	}

	template <Solid::TScriptStructConcept T>
	static NO_DISCARD FFlecsRecordPairSlot Make()
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		OutSlot.PairScriptStruct = FInstancedStruct::Make<T>();
		return OutSlot;
	}

	template <Solid::TScriptStructConcept T>
	static NO_DISCARD FFlecsRecordPairSlot Make(const T& InValue)
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		OutSlot.PairScriptStruct = FInstancedStruct::Make<T>(InValue);
		return OutSlot;
	}
	
	static NO_DISCARD FFlecsRecordPairSlot Make(const FFlecsId InEntityHandle)
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::EntityHandle;
		OutSlot.EntityHandle = InEntityHandle;
		return OutSlot;
	}

	static NO_DISCARD FFlecsRecordPairSlot Make(const FGameplayTag& InGameplayTag)
	{
		FFlecsRecordPairSlot OutSlot;
		OutSlot.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		OutSlot.GameplayTag = InGameplayTag;
		return OutSlot;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree")
	EFlecsPairNodeType PairNodeType = EFlecsPairNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct PairScriptStruct;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
	//	meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::ScriptEnum", EditConditionHides))
	//FSolidEnumSelector PairScriptEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::EntityHandle", EditConditionHides))
	FFlecsId EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "PairNodeType == EFlecsPairNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsRecordPairSlot& Other) const
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

		UNREACHABLE;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsRecordPairSlot& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsPairSlot

UENUM(BlueprintType)
enum class EFlecsValuePairType : uint8
{
	None = 0,
	First = 1,
	Second = 2,
}; // enum class EFlecsValuePairType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRecordPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree")
	FFlecsRecordPairSlot First;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree")
	FFlecsRecordPairSlot Second;

	// @TODO: make an edit condition for this?
	/**
	 * @brief Only needed if either First or Second are Instanced Structs
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "First.PairNodeType == EFlecsPairNodeType::ScriptStruct || Second.PairNodeType == EFlecsPairNodeType::ScriptStruct",
		EditConditionHides))
	EFlecsValuePairType PairValueType = EFlecsValuePairType::None;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsRecordPair& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsRecordPair& Other) const
	{
		return !(*this == Other);
	}
	
	void AddToEntity(const FFlecsEntityHandle& InEntityHandle) const;

private:
	
	template <Unreal::Flecs::CRecordPairSlotType T>
	NO_DISCARD FFlecsRecordPairSlot MakeSlot(T&& InValue) const
	{
		FFlecsRecordPairSlot OutSlot;
		
		if constexpr (std::is_convertible<T, FInstancedStruct>::value)
		{
			OutSlot.PairNodeType = EFlecsPairNodeType::ScriptStruct;
			OutSlot.PairScriptStruct = MoveTemp(InValue);
		}
		else if constexpr (std::is_convertible<T, FFlecsId>::value)
		{
			OutSlot.PairNodeType = EFlecsPairNodeType::EntityHandle;
			OutSlot.EntityHandle = MoveTemp(InValue);
		}
		else if constexpr (std::is_convertible<T, FGameplayTag>::value)
		{
			OutSlot.PairNodeType = EFlecsPairNodeType::FGameplayTag;
			OutSlot.GameplayTag = MoveTemp(InValue);
		}
		else
		{
			static_assert(false, "Type T is not a valid Record Pair Slot Type");
		}
		
		return OutSlot;
	}

}; // struct FFlecsPair

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsComponentTypeInfo final
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree")
	EFlecsComponentNodeType NodeType = EFlecsComponentNodeType::ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptStruct", EditConditionHides))
	FInstancedStruct ScriptStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::ScriptEnum", EditConditionHides))
	FSolidEnumSelector ScriptEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::EntityHandle", EditConditionHides))
	FFlecsId EntityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::FGameplayTag", EditConditionHides))
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Tree",
		meta = (EditCondition = "NodeType == EFlecsComponentNodeType::Pair", EditConditionHides))
	FFlecsRecordPair Pair;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsComponentTypeInfo& Other) const
	{
		switch (NodeType)
		{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					return NodeType == Other.NodeType && ScriptStruct == Other.ScriptStruct && Pair == Other.Pair;
				}
			case EFlecsComponentNodeType::ScriptEnum:
				{
					return NodeType == Other.NodeType
						&& ScriptEnum.Class == Other.ScriptEnum.Class && ScriptEnum.Value == Other.ScriptEnum.Value;
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

		UNREACHABLE;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsComponentTypeInfo& Other) const
	{
		return !(*this == Other);
	}
	
}; // struct FFlecsComponentTypeInfo

/**
 * @brief A record of a generic entity's components and sub-entities,
 * this can be applied to an actual entity to give it the same components/sub-entities.
 */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityRecord
{
	GENERATED_BODY()

	FORCEINLINE FFlecsEntityRecord() = default;
	virtual ~FFlecsEntityRecord() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Record")
	TArray<FFlecsComponentTypeInfo> Components;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Record")
	TArray<TInstancedStruct<FFlecsEntityRecord>> SubEntities;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsEntityRecord& Other) const
	{
		return Components == Other.Components;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsEntityRecord& Other) const
	{
		return !(*this == Other);
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE FFlecsEntityRecord& AddComponent()
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>();
		Components.Add(NewComponent);

		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE FFlecsEntityRecord& AddComponent(const T& InComponent)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>(InComponent);
		Components.Add(NewComponent);

		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE FFlecsEntityRecord& AddComponent(T&& InComponent)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>(MoveTemp(InComponent));
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(const FFlecsId InEntityHandle)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::EntityHandle;
		NewComponent.EntityHandle = InEntityHandle;
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(const FGameplayTag& InGameplayTag)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::FGameplayTag;
		NewComponent.GameplayTag = InGameplayTag;
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(FGameplayTag&& InGameplayTag)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::FGameplayTag;
		NewComponent.GameplayTag = MoveTemp(InGameplayTag);
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(const FSolidEnumSelector& InScriptEnum)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptEnum;
		NewComponent.ScriptEnum = InScriptEnum;
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(FSolidEnumSelector&& InScriptEnum)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptEnum;
		NewComponent.ScriptEnum = MoveTemp(InScriptEnum);
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(const FFlecsRecordPair& InPair)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::Pair;
		NewComponent.Pair = InPair;
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE FFlecsEntityRecord& AddComponent(FFlecsRecordPair&& InPair)
	{
		FFlecsComponentTypeInfo NewComponent;
		NewComponent.NodeType = EFlecsComponentNodeType::Pair;
		NewComponent.Pair = MoveTemp(InPair);
		Components.Add(NewComponent);

		return *this;
	}

	FORCEINLINE int32 AddSubEntity(const TInstancedStruct<FFlecsEntityRecord>& InSubEntity)
	{
		return SubEntities.Add(InSubEntity);
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

	NO_DISCARD FORCEINLINE bool HasSubEntities() const
	{
		return !SubEntities.IsEmpty();
	}

	NO_DISCARD FORCEINLINE int32 GetSubEntityCount() const
	{
		return SubEntities.Num();
	}

	FORCEINLINE TConstStructView<FFlecsEntityRecord> GetSubEntity(const int32 InIndex) const
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		return SubEntities[InIndex];
	}

	FORCEINLINE TStructView<FFlecsEntityRecord> GetSubEntity(const int32 InIndex)
	{
		solid_checkf(SubEntities.IsValidIndex(InIndex), TEXT("Index is out of bounds"));
		return SubEntities[InIndex];
	}

	virtual void ApplyRecordToEntity(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld, const FFlecsEntityHandle& InEntityHandle) const;

}; // struct FFlecsEntityRecord

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityRecordComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Record")
	TInstancedStruct<FFlecsEntityRecord> EntityRecord;
	
}; // struct FFlecsEntityRecordComponent

REGISTER_FLECS_COMPONENT(FFlecsEntityRecordComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

	