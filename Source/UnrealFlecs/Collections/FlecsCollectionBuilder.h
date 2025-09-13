// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"

#include "FlecsCollectionBuilder.generated.h"

struct FFlecsCollectionBuilder;

struct UNREALFLECS_API FFlecsPairBuilder
{
	FFlecsRecordPair Pair;
	
	FORCEINLINE FFlecsPairBuilder& ValueIs(EFlecsValuePairType InType)
	{
		Pair.PairValueType = InType;
		return *this;
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsPairBuilder& First()
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>();
		
		return *this;
	}
	
	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsPairBuilder& First(const T& ScriptStructValue)
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		
		return *this;
	}

	FORCEINLINE FFlecsPairBuilder& First(const FFlecsId& InEntityId)
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.First.EntityHandle = InEntityId;
		
		return *this;
	}

	FORCEINLINE FFlecsPairBuilder& Add(const FGameplayTag& InTag)
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		Pair.First.GameplayTag = InTag;
		
		return *this;
	}
	
	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsPairBuilder& Second(const T& ScriptStructValue)
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.Second.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		return *this;
	}

	FORCEINLINE FFlecsPairBuilder& Second(const FFlecsId& InEntityId)
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.Second.EntityHandle = InEntityId;
		
		return *this;
	}
	
	FORCEINLINE FFlecsPairBuilder& Second(const FGameplayTag& InTag)
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		Pair.Second.GameplayTag = InTag;
		
		return *this;
	}
	
}; // struct FFlecsPairBuilder

USTRUCT()
struct UNREALFLECS_API FFlecsSubEntityBuilder
{
	GENERATED_BODY()
public:
	FFlecsCollectionBuilder* ParentBuilder = nullptr;
	FFlecsRecordSubEntity SubEntity;

	FORCEINLINE explicit FFlecsSubEntityBuilder(FFlecsCollectionBuilder& InParentBuilder)
		: ParentBuilder(&InParentBuilder)
	{
	}

	FORCEINLINE FFlecsSubEntityBuilder& Name(const FString& InName)
	{
		SubEntity.Name = InName;
		return *this;
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsSubEntityBuilder& Add()
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>();
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsSubEntityBuilder& Add(const T& InComponent)
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>(InComponent);
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& Add(const FFlecsEntityHandle& InEntityHandle)
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::EntityHandle;
		NewComponent.EntityHandle = InEntityHandle;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& Add(const FGameplayTag& InGameplayTag)
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::FGameplayTag;
		NewComponent.GameplayTag = InGameplayTag;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& Add(const FFlecsRecordPair& InPair)
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::Pair;
		NewComponent.Pair = InPair;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& Add(const FFlecsId& InId)
	{
		FFlecsComponentTypeInfo Info;
		
		Info.NodeType = EFlecsComponentNodeType::EntityHandle;
		Info.EntityHandle = InId;
		
		SubEntity.Components.Add(MoveTemp(Info));
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& ReferenceCollection(const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset, bool bMarkAsSlot)
	{
		FFlecsCollectionReferenceComponent Ref;
		Ref.Collection.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Collection.Asset = InAsset;
		
		// Add ref component
		{
			FFlecsComponentTypeInfo Info;
			
			Info.NodeType = EFlecsComponentNodeType::ScriptStruct;
			Info.ScriptStruct = FInstancedStruct::Make<FFlecsCollectionReferenceComponent>(Ref);
			
			SubEntity.Components.Add(MoveTemp(Info));
		}

		if (bMarkAsSlot)
		{
			FFlecsComponentTypeInfo Info;
			
			Info.NodeType = EFlecsComponentNodeType::ScriptStruct;
			Info.ScriptStruct = FInstancedStruct::Make<FFlecsCollectionSlotTag>(FFlecsCollectionSlotTag{});
			
			SubEntity.Components.Add(MoveTemp(Info));
		}
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder& MarkSlot()
	{
		FFlecsComponentTypeInfo Info;
		
		Info.NodeType = EFlecsComponentNodeType::ScriptStruct;
		Info.ScriptStruct = FInstancedStruct::Make<FFlecsCollectionSlotTag>(FFlecsCollectionSlotTag{});
		
		SubEntity.Components.Add(MoveTemp(Info));
		
		return *this;
	}

	FFlecsCollectionBuilder& End() const;
	
}; // struct FFlecsSubEntityBuilder

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionBuilder
{
	GENERATED_BODY()
public:
	FName IdName;
	
	FFlecsEntityRecord Record;
	TArray<FFlecsCollectionReference> Collections;

	NO_DISCARD static FORCEINLINE FFlecsCollectionBuilder Create()
	{
		return FFlecsCollectionBuilder();
	}

	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsCollectionBuilder& Add()
	{
		Record.AddComponent<T>();
		
		return *this;
	}
	
	template <Solid::TStaticStructConcept T>
	FORCEINLINE FFlecsCollectionBuilder& Add(const T& InComponent)
	{
		Record.AddComponent<T>(InComponent);
		
		return *this;
	}

	FORCEINLINE FFlecsCollectionBuilder& Add(const FFlecsId InId)
	{
		Record.AddComponent(InId);
		
		return *this;
	}

	FORCEINLINE FFlecsCollectionBuilder& Add(const FGameplayTag& InGameplayTag)
	{
		Record.AddComponent(InGameplayTag);
		
		return *this;
	}

	FORCEINLINE FFlecsCollectionBuilder& Add(const FFlecsRecordPair& InPair)
	{
		Record.AddComponent(InPair);
		
		return *this;
	}

	FORCEINLINE FFlecsSubEntityBuilder MakeSubEntity()
	{
		return FFlecsSubEntityBuilder(*this);
	}

	FORCEINLINE FFlecsCollectionBuilder& ReferenceCollection(const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Asset = InAsset;
		
		Collections.Add(Ref);
		
		return *this;
	}

	FORCEINLINE FFlecsCollectionBuilder& ReferenceCollection(const FFlecsCollectionId& InId)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Id;
		
		Collections.Add(Ref);
		
		return *this;
	}

	template <Solid::TClassConcept T>
	FORCEINLINE FFlecsCollectionBuilder& ReferenceCollection()
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::UClass;
		Ref.Class = T::StaticClass();

		Collections.Add(Ref);

		return *this;
	}

	FORCEINLINE FFlecsCollectionBuilder& Name(const FName& InName)
	{
		IdName = InName;
		
		return *this;
	}
	
}; // struct FFlecsCollectionBuilder