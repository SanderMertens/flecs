// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsCollectionDefinition.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"

#include "FlecsCollectionBuilder.generated.h"

struct FFlecsCollectionBuilder;

struct UNREALFLECS_API FFlecsCollectionPairBuilder
{
	mutable FFlecsRecordPair Pair;
	
	FORCEINLINE const FFlecsCollectionPairBuilder& ValueIs(const EFlecsValuePairType InType) const
	{
		Pair.PairValueType = InType;
		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsCollectionPairBuilder& First() const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>();
		
		return *this;
	}
	
	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsCollectionPairBuilder& First(const T& ScriptStructValue) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionPairBuilder& First(const FFlecsId& InEntityId) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.First.EntityHandle = InEntityId;
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionPairBuilder& Add(const FGameplayTag& InTag) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		Pair.First.GameplayTag = InTag;
		
		return *this;
	}
	
	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsCollectionPairBuilder& Second(const T& ScriptStructValue) const
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.Second.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		return *this;
	}

	FORCEINLINE const FFlecsCollectionPairBuilder& Second(const FFlecsId& InEntityId) const
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.Second.EntityHandle = InEntityId;
		
		return *this;
	}
	
	FORCEINLINE const FFlecsCollectionPairBuilder& Second(const FGameplayTag& InTag) const
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		Pair.Second.GameplayTag = InTag;
		
		return *this;
	}
	
}; // struct FFlecsPairBuilder

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSubEntityCollectionBuilder
{
	GENERATED_BODY()

public:
	FFlecsSubEntityCollectionBuilder() = default;
	FFlecsSubEntityCollectionBuilder(const FString& InIdName, const FFlecsEntityRecord& InRecord)
		: IdName(InIdName), Record(InRecord)
	{
	}

	
	

	UPROPERTY()
	FString IdName;
	
	UPROPERTY()
	FFlecsEntityRecord Record;
	
}; // struct FFlecsSubEntityCollectionBuilder

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionBuilder
{
	GENERATED_BODY()

	NO_DISCARD static FORCEINLINE FFlecsCollectionBuilder Create(FFlecsCollectionDefinition& InDefinition)
	{
		return FFlecsCollectionBuilder(InDefinition);
	}
	
public:
	FORCEINLINE FFlecsCollectionBuilder()
		: CollectionDefinition(nullptr)
	{
	}

	FORCEINLINE explicit FFlecsCollectionBuilder(FFlecsCollectionDefinition& InDefinition)
		: CollectionDefinition(&InDefinition)
	{
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsCollectionBuilder& Add() const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent<T>();
		
		return *this;
	}
	
	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsCollectionBuilder& Add(const T& InComponent) const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent<T>(InComponent);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& Add(const FFlecsId InId) const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent(InId);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& Add(const FGameplayTag& InGameplayTag) const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent(InGameplayTag);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& Add(const FFlecsRecordPair& InPair) const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent(InPair);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& AddPair(const FFlecsRecordPair& InPair) const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent(InPair);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection(const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset,
		const FInstancedStruct& InParameters = FInstancedStruct()) const
	{
		FFlecsCollectionInstanceReference Ref;
		Ref.Collection.Asset = InAsset;
		Ref.Collection.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Parameters = InParameters;
		
		GetCollectionDefinition().Collections.Add(Ref);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection(const FFlecsCollectionId& InId, const FInstancedStruct& InParameters = FInstancedStruct()) const
	{
		FFlecsCollectionInstanceReference Ref;
		Ref.Collection.Id = InId;
		Ref.Collection.Mode = EFlecsCollectionReferenceMode::Id;
		Ref.Parameters = InParameters;
		
		GetCollectionDefinition().Collections.Add(Ref);
		
		return *this;
	}

	template <Solid::TStaticClassConcept T>
	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection(const FInstancedStruct& InParameters = FInstancedStruct()) const
	{
		FFlecsCollectionInstanceReference Ref;
		Ref.Collection.Mode = EFlecsCollectionReferenceMode::UClass;
		Ref.Collection.Class = T::StaticClass();
		Ref.Parameters = InParameters;

		GetCollectionDefinition().Collections.Add(Ref);

		return *this;
	}
	
	FORCEINLINE const FFlecsCollectionBuilder& Name(const FString& InName) const
	{
		IdName = InName;
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& Parameters(const FInstancedStruct& InParameters,
		const FFlecsCollectionParametersComponent::FApplyParametersFunction& InApplyFunction) const
	{
		solid_cassume(CollectionDefinition);

		FFlecsCollectionParametersComponent ParametersComponent;
		ParametersComponent.ParameterType = InParameters;
		ParametersComponent.ApplyParametersFunction = InApplyFunction;
		
		GetCollectionDefinition().Record.AddComponent<FFlecsCollectionParametersComponent>(MoveTemp(ParametersComponent));
		
		return *this;
	}

	template <Solid::TScriptStructConcept T, typename TApplyFunction>
	requires (!std::is_same<T, FInstancedStruct>::value)
	FORCEINLINE const FFlecsCollectionBuilder& Parameters(const T& InParameters, TApplyFunction&& InApplyFunction) const
	{
		solid_cassume(CollectionDefinition);

		FFlecsCollectionParametersComponent ParametersComponent;
		ParametersComponent.ParameterType = FInstancedStruct::Make<T>(InParameters);
		ParametersComponent.ApplyParametersFunction = [InApplyFunction = std::forward<TApplyFunction>(InApplyFunction)]
			(FFlecsEntityHandle TargetEntity, const FInstancedStruct& Parameters)
		{
			InApplyFunction(TargetEntity, Parameters.Get<T>());
		};
		
		GetCollectionDefinition().Record.AddComponent<FFlecsCollectionParametersComponent>(MoveTemp(ParametersComponent));
		
		return *this;
	}

	// @TODO: add other relationship overloads?
	
	void MarkSlot() const
	{
		solid_cassume(CollectionDefinition);
		
		GetCollectionDefinition().Record.AddComponent<FFlecsCollectionSlotTag>();
	}

	NO_DISCARD FORCEINLINE FFlecsCollectionDefinition& GetCollectionDefinition() const
	{
		solid_cassume(CollectionDefinition);
		
		return *CollectionDefinition;
	}

	FFlecsCollectionBuilder* ParentBuilder = nullptr;

	UPROPERTY()
	mutable FString IdName;
	
	mutable FFlecsCollectionDefinition* CollectionDefinition;
	
}; // struct FFlecsCollectionBuilder