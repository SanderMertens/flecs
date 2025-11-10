// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsCollectionDefinition.h"

#include "Entities/FlecsEntityRecord.h"
#include "FlecsCollectionTypes.h"

#include "FlecsCollectionBuilder.generated.h"

struct FFlecsCollectionBuilder;

struct UNREALFLECS_API FFlecsPairBuilder
{
	mutable FFlecsRecordPair Pair;
	
	FORCEINLINE const FFlecsPairBuilder& ValueIs(const EFlecsValuePairType InType) const
	{
		Pair.PairValueType = InType;
		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsPairBuilder& First() const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>();
		
		return *this;
	}
	
	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsPairBuilder& First(const T& ScriptStructValue) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.First.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		
		return *this;
	}

	FORCEINLINE const FFlecsPairBuilder& First(const FFlecsId& InEntityId) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.First.EntityHandle = InEntityId;
		
		return *this;
	}

	FORCEINLINE const FFlecsPairBuilder& Add(const FGameplayTag& InTag) const
	{
		Pair.First.PairNodeType = EFlecsPairNodeType::FGameplayTag;
		Pair.First.GameplayTag = InTag;
		
		return *this;
	}
	
	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsPairBuilder& Second(const T& ScriptStructValue) const
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::ScriptStruct;
		Pair.Second.PairScriptStruct = FInstancedStruct::Make<T>(ScriptStructValue);
		return *this;
	}

	FORCEINLINE const FFlecsPairBuilder& Second(const FFlecsId& InEntityId) const
	{
		Pair.Second.PairNodeType = EFlecsPairNodeType::EntityHandle;
		Pair.Second.EntityHandle = InEntityId;
		
		return *this;
	}
	
	FORCEINLINE const FFlecsPairBuilder& Second(const FGameplayTag& InTag) const
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
	FORCEINLINE FFlecsSubEntityBuilder()
	{
	}

	FORCEINLINE explicit FFlecsSubEntityBuilder(const FFlecsCollectionBuilder& InParentBuilder)
		: ParentBuilder(&InParentBuilder)
	{
	}

	FORCEINLINE const FFlecsSubEntityBuilder& Name(const FString& InName) const
	{
		SubEntity.Name = InName;
		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsSubEntityBuilder& Add() const
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>();
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	template <Solid::TScriptStructConcept T>
	FORCEINLINE const FFlecsSubEntityBuilder& Add(const T& InComponent) const
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::ScriptStruct;
		NewComponent.ScriptStruct = FInstancedStruct::Make<T>(InComponent);
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE const FFlecsSubEntityBuilder& Add(const FFlecsEntityHandle& InEntityHandle) const
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::EntityHandle;
		NewComponent.EntityHandle = InEntityHandle;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE const FFlecsSubEntityBuilder& Add(const FGameplayTag& InGameplayTag) const
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::FGameplayTag;
		NewComponent.GameplayTag = InGameplayTag;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE const FFlecsSubEntityBuilder& Add(const FFlecsRecordPair& InPair) const
	{
		FFlecsComponentTypeInfo NewComponent;
		
		NewComponent.NodeType = EFlecsComponentNodeType::Pair;
		NewComponent.Pair = InPair;
		
		SubEntity.Components.Add(NewComponent);
		
		return *this;
	}

	FORCEINLINE const FFlecsSubEntityBuilder& Add(const FFlecsId& InId) const
	{
		FFlecsComponentTypeInfo Info;
		
		Info.NodeType = EFlecsComponentNodeType::EntityHandle;
		Info.EntityHandle = InId;
		
		SubEntity.Components.Add(MoveTemp(Info));
		
		return *this;
	}

	/*FORCEINLINE FFlecsSubEntityBuilder& ReferenceCollection(
		const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset, bool bMarkAsSlot)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Asset = InAsset;
		
		SubEntity.Collections.Add(Ref);

		if (bMarkAsSlot)
		{
			MarkSlot();
		}
		
		return *this;
	}*/

	FORCEINLINE const FFlecsSubEntityBuilder& MarkSlot() const
	{
		FFlecsComponentTypeInfo Info;
		
		Info.NodeType = EFlecsComponentNodeType::ScriptStruct;
		Info.ScriptStruct = FInstancedStruct::Make<FFlecsCollectionSlotTag>(FFlecsCollectionSlotTag{});
		
		SubEntity.Components.Add(MoveTemp(Info));
		
		return *this;
	}

	const FFlecsCollectionBuilder& End() const;

	const FFlecsCollectionBuilder* ParentBuilder = nullptr;
	mutable FFlecsRecordSubEntity SubEntity;
	
}; // struct FFlecsSubEntityBuilder

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

	FORCEINLINE FFlecsSubEntityBuilder MakeSubEntity() const
	{
		return FFlecsSubEntityBuilder(*this);
	}

	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection(const TSolidNotNull<UFlecsCollectionDataAsset*> InAsset) const
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Asset = InAsset;
		
		GetCollectionDefinition().Collections.Add(Ref);
		
		return *this;
	}

	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection(const FFlecsCollectionId& InId) const
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Id;
		Ref.Id = InId;
		
		GetCollectionDefinition().Collections.Add(Ref);
		
		return *this;
	}

	template <Solid::TStaticClassConcept T>
	FORCEINLINE const FFlecsCollectionBuilder& ReferenceCollection() const
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::UClass;
		Ref.Class = T::StaticClass();

		GetCollectionDefinition().Collections.Add(Ref);

		return *this;
	}
	
	FORCEINLINE const FFlecsCollectionBuilder& Name(const FName& InName) const
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

	NO_DISCARD FORCEINLINE FFlecsCollectionDefinition& GetCollectionDefinition() const
	{
		solid_cassume(CollectionDefinition);
		
		return *CollectionDefinition;
	}

	UPROPERTY()
	mutable FName IdName;
	
	mutable FFlecsCollectionDefinition* CollectionDefinition;
	
}; // struct FFlecsCollectionBuilder