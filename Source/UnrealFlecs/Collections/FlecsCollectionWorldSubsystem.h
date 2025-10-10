// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsCollectionBuilder.h"
#include "FlecsCollectionDefinition.h"
#include "FlecsCollectionId.h"

#include "Worlds/FlecsAbstractWorldSubsystem.h"

#include "FlecsCollectionWorldSubsystem.generated.h"

class IFlecsCollectionInterface;
class UFlecsCollectionDataAsset;

namespace Unreal::Flecs::Collections
{
	template <typename FuncType>
	concept TCollectionBuilderFunc = requires(FuncType Func, FFlecsCollectionBuilder& Builder)
	{
		{ Func(Builder) } -> std::same_as<void>;
	}; // concept TCollectionBuilderFunc
	
} // namespace Unreal::Flecs::Collections

// @TODO: fix this shit dont use rn
UCLASS()
class UNREALFLECS_API UFlecsCollectionWorldSubsystem final : public UFlecsAbstractWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle RegisterCollectionAsset(const UFlecsCollectionDataAsset* InAsset);

	FFlecsEntityHandle RegisterCollectionDefinition(const FName& InName, const FFlecsCollectionDefinition& InDefinition);
	FFlecsEntityHandle RegisterCollectionClass(const TSolidNotNull<UClass*> InClass, const FFlecsCollectionBuilder& InBuilder);
	FFlecsEntityHandle RegisterCollectionClass(const TSolidNotNull<TScriptInterface<IFlecsCollectionInterface>*> InInterfaceObject);

	template <Solid::TStaticClassConcept T>
	FORCEINLINE FFlecsEntityHandle RegisterTypedCollection()
	{
		static_assert(TIsDerivedFrom<T, IFlecsCollectionInterface>::Value,
		              "T must implement IFlecsCollectionInterface");
		return RegisterCollectionClass(T::StaticClass());
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByAsset(const UFlecsCollectionDataAsset* Asset) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabById(const FFlecsCollectionId& Id) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByClass(const TSubclassOf<UObject> InClass) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FORCEINLINE FFlecsEntityHandle GetCollectionScope() const
	{
		return CollectionScopeEntity;
	}

	template <Unreal::Flecs::Collections::TCollectionBuilderFunc FuncType>
	FFlecsEntityHandle RegisterCollectionBuilder(FuncType&& InBuildFunc)
	{
		FFlecsCollectionDefinition Definition;

		FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Definition);
		
		InBuildFunc(Builder);

		return RegisterCollectionDefinition(Builder.IdName, Definition);
	}

	template <Unreal::Flecs::Collections::TCollectionBuilderFunc FuncType>
	FFlecsEntityHandle RegisterCollectionClass(const TSolidNotNull<UClass*> InClass, FuncType&& InBuildFunc)
	{
		FFlecsCollectionDefinition Definition;
		
		FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Definition);
		InBuildFunc(Builder);
		
		return RegisterCollectionClass(InClass, Builder);
	}

private:
	// Ensure a shell entity exists for the collection (without any components, just the id)
	NO_DISCARD FFlecsEntityHandle EnsurePrefabShell(const FFlecsCollectionId& Id);

	// Create/Find the collection entity referenced in the @Reference
	NO_DISCARD FFlecsEntityHandle ResolveCollectionReference(const FFlecsCollectionReference& Reference);

	// Make sure any Collection References Child Entities may have of this collection are cleaned up/created (is called recursively)
	void ExpandChildCollectionReferences(const FFlecsEntityHandle& InCollectionEntity);

	NO_DISCARD FFlecsEntityHandle CreatePrefabEntity(const FString& Name, const FFlecsEntityRecord& Record) const;
	NO_DISCARD FFlecsEntityHandle CreatePrefabEntity(const TSolidNotNull<UClass*> InClass,
		const FFlecsEntityRecord& Record) const;
	
	NO_DISCARD bool ClassImplementsCollectionInterface(const TSolidNotNull<const UClass*> InClass) const;

	UPROPERTY()
	TMap<FFlecsCollectionId, FFlecsEntityHandle> RegisteredCollections;

	// Recursion guard
	TSet<FFlecsCollectionId> InProgressCollections;
	
	FDelegateHandle AssetAddedHandle;
	FDelegateHandle AssetRemovedHandle;

	UPROPERTY()
	FFlecsEntityHandle CollectionScopeEntity;
	
}; // class UFlecsCollectionWorldSubsystem
