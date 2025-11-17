// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "StructUtils/PropertyBag.h"

#include "Properties/FlecsComponentProperties.h"
#include "FlecsCollectionId.h"

#include "FlecsCollectionTypes.generated.h"

class UFlecsCollectionDataAsset;
class UFlecsCollectionClass;

UENUM(BlueprintType)
enum class EFlecsCollectionReferenceMode : uint8
{
	Asset = 0,
	Id = 1,
	UClass = 2,
	
}; // enum class EFlecsCollectionReferenceMode

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionReference
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionReference() = default;

	static NO_DISCARD FFlecsCollectionReference FromAsset(const TSolidNotNull<const UFlecsCollectionDataAsset*> InAsset)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Asset;
		Ref.Asset = InAsset;
		return Ref;
	}

	// @TODO: maybe validate param?
	static NO_DISCARD FFlecsCollectionReference FromClass(const TSubclassOf<UObject> InClass)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::UClass;
		Ref.Class = InClass;
		return Ref;
	}

	static NO_DISCARD FFlecsCollectionReference FromId(const FFlecsCollectionId& InId)
	{
		FFlecsCollectionReference Ref;
		Ref.Mode = EFlecsCollectionReferenceMode::Id;
		Ref.Id = InId;
		return Ref;
	}

	static NO_DISCARD FFlecsCollectionReference FromId(const FString& InIdString)
	{
		return FromId(FFlecsCollectionId(InIdString));
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	EFlecsCollectionReferenceMode Mode = EFlecsCollectionReferenceMode::Asset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::Asset", EditConditionHides))
	TObjectPtr<const UFlecsCollectionDataAsset> Asset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::UClass", EditConditionHides,
			MustImplement = "/Script/UnrealFlecs.FlecsCollectionInterface"))
	TSubclassOf<UObject> Class;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs",
		meta = (EditCondition = "Mode == EFlecsCollectionReferenceMode::Id", EditConditionHides))
	FFlecsCollectionId Id;
	
}; // struct FFlecsCollectionReference
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionInstancedReference
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionInstancedReference() = default;

	FORCEINLINE FFlecsCollectionInstancedReference(
			const FFlecsCollectionReference& InCollection, const FInstancedStruct& InParameters = FInstancedStruct())
		: Collection(InCollection)
		, Parameters(InParameters)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs", meta = (ShowOnlyInnerProperties))
	FFlecsCollectionReference Collection;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	FInstancedStruct Parameters;
	
}; // struct FFlecsCollectionInstancedReference

/** compose another Collection by reference (adds (IsA, @param Collection) in compile-time and then removes itself). */
USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionReferenceComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsCollectionReferenceComponent() = default;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flecs")
	TArray<FFlecsCollectionInstancedReference> Collections;
	
}; // struct FFlecsCollectionReferenceComponent

REGISTER_FLECS_COMPONENT(FFlecsCollectionReferenceComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.Add(flecs::Sparse)
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionPrefabTag
{
	GENERATED_BODY()
}; // struct FFlecsCollectionPrefabTag

REGISTER_FLECS_COMPONENT(FFlecsCollectionPrefabTag,
[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit)
			.AddPair(flecs::With, flecs::Prefab);
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsCollectionSlotTag
{
	GENERATED_BODY()
}; // struct FFlecsCollectionSlotTag

REGISTER_FLECS_COMPONENT(FFlecsCollectionSlotTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSubEntityIndex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	int32 Index = INDEX_NONE;
};

REGISTER_FLECS_COMPONENT(FFlecsSubEntityIndex,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		// Sparse is fine; we just need a storage component for lookup.
		InComponentHandle.Add(flecs::Sparse);
	});

// @TODO: maybe add an OnSet Event like in templates

USTRUCT()
struct UNREALFLECS_API FFlecsCollectionParametersComponent
{
	GENERATED_BODY()

	using FApplyParametersFunction = std::function<void(const FFlecsEntityHandle&, const FInstancedStruct&)>;

public:
	UPROPERTY()
	FInstancedStruct ParameterType;

	FApplyParametersFunction ApplyParametersFunction;

	template <Solid::TScriptStructConcept T, typename FuncType>
	void SetApplyParametersFunction(FuncType&& InFunction)
	{
		ApplyParametersFunction = [InFunction = std::forward<FuncType>(InFunction)]
			(const FFlecsEntityHandle& InEntityHandle, const FInstancedStruct& InParameters)
		{
			std::invoke(InFunction, InEntityHandle,
				TInstancedStruct<T>::InitializeAsScriptStruct(InParameters.GetScriptStruct(), InParameters.GetMemory()));
		};
	}

	void ApplyParameters(const FFlecsEntityHandle& InEntityHandle, const FInstancedStruct& InParameters) const;
	
}; // struct FFlecsCollectionParametersComponent

REGISTER_FLECS_COMPONENT(FFlecsCollectionParametersComponent,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle
			.AddPair(flecs::OnInstantiate, flecs::DontInherit);
	});