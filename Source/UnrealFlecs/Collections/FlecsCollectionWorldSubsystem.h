// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsCollectionBuilder.h"
#include "FlecsCollectionDefinition.h"
#include "FlecsCollectionId.h"

#include "Worlds/FlecsAbstractWorldSubsystem.h"

#include "FlecsCollectionWorldSubsystem.generated.h"

class UFlecsCollectionWorldSubsystem;
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

USTRUCT()
struct UNREALFLECS_API FFlecsCollectionSubsystemSingleton
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TWeakObjectPtr<UFlecsCollectionWorldSubsystem> WorldSubsystem;
	
}; // struct FFlecsCollectionSubsystemSingleton

REGISTER_FLECS_COMPONENT(FFlecsCollectionSubsystemSingleton,
		[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
		{
			InComponentHandle
				.Add(flecs::Singleton);
		});

UCLASS()
class UNREALFLECS_API UFlecsCollectionWorldSubsystem final : public UFlecsAbstractWorldSubsystem
{
	GENERATED_BODY()

public:
	UFlecsCollectionWorldSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnFlecsWorldInitialized(const TSolidNotNull<UFlecsWorld*> InWorld) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle RegisterCollectionAsset(const UFlecsCollectionDataAsset* InAsset);

	FFlecsEntityHandle RegisterCollectionDefinition(const FString& InName, FFlecsCollectionDefinition& InDefinition);
	FFlecsEntityHandle RegisterCollectionClass(const TSolidNotNull<UClass*> InClass, const FFlecsCollectionBuilder& InBuilder);
	FFlecsEntityHandle RegisterCollectionInterfaceClass(const TSolidNotNull<UClass*> InInterfaceObject);

	template <Solid::TStaticClassConcept T>
	FORCEINLINE FFlecsEntityHandle RegisterCollectionInterfaceClass()
	{
		static_assert(TIsDerivedFrom<T, IFlecsCollectionInterface>::Value,
		              "T must implement IFlecsCollectionInterface");
		return RegisterCollectionInterfaceClass(T::StaticClass());
	}
	
	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByAsset(const UFlecsCollectionDataAsset* Asset) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByIdRaw(const FFlecsId& Id) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByCollectionId(const FFlecsCollectionId& Id) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetPrefabByClass(const TSubclassOf<UObject> InClass) const;

	UFUNCTION(BlueprintCallable, Category = "Flecs|Collections")
	FFlecsEntityHandle GetCollectionScope() const;

	template <Unreal::Flecs::Collections::TCollectionBuilderFunc FuncType>
	FFlecsEntityHandle RegisterCollectionBuilder(FuncType&& InBuildFunc)
	{
		FFlecsCollectionDefinition Definition;

		FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Definition);

		std::invoke(std::forward<FuncType>(InBuildFunc), Builder);

		return RegisterCollectionDefinition(Builder.IdName, Definition);
	}

	template <Unreal::Flecs::Collections::TCollectionBuilderFunc FuncType>
	FFlecsEntityHandle RegisterCollectionClass(const TSolidNotNull<UClass*> InClass, FuncType&& InBuildFunc)
	{
		checkf(!ClassImplementsCollectionInterface(InClass),
		       TEXT("Use RegisterCollectionInterfaceClass to register collection classes that implement IFlecsCollectionInterface"));
		
		FFlecsCollectionDefinition Definition;
		
		FFlecsCollectionBuilder Builder = FFlecsCollectionBuilder::Create(Definition);
		std::invoke(std::forward<FuncType>(InBuildFunc), Builder);
		
		return RegisterCollectionClass(InClass, Builder);
	}

	void AddCollectionToEntity(const FFlecsEntityHandle& InEntity, const FFlecsId InCollectionId,
		const FInstancedStruct& InParameters = FInstancedStruct());
	
	void AddCollectionToEntity(const FFlecsEntityHandle& InEntity, const FFlecsCollectionId& InCollectionId,
		const FInstancedStruct& InParameters = FInstancedStruct());
	
	void AddCollectionToEntity(const FFlecsEntityHandle& InEntity, const FFlecsCollectionReference& InCollectionReference,
		const FInstancedStruct& InParameters = FInstancedStruct());
	
	void AddCollectionToEntity(const FFlecsEntityHandle& InEntity, const TSolidNotNull<const UFlecsCollectionDataAsset*> InAsset,
		const FInstancedStruct& InParameters = FInstancedStruct());
	
	void AddCollectionToEntity(const FFlecsEntityHandle& InEntity, const TSubclassOf<UObject> InClass,
		const FInstancedStruct& InParameters = FInstancedStruct());

	template <Solid::TStaticClassConcept T>
	FORCEINLINE void AddCollectionToEntity(const FFlecsEntityHandle& InEntity)
	{
		static_assert(TIsDerivedFrom<T, IFlecsCollectionInterface>::Value,
		              "T must implement IFlecsCollectionInterface");
		AddCollectionToEntity(InEntity, T::StaticClass());
	}
	
	void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity, const FFlecsCollectionId& InCollectionId);
	void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity, const FFlecsCollectionReference& InCollectionReference);
	void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity, const TSolidNotNull<const UFlecsCollectionDataAsset*> InAsset);
	void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity, const TSubclassOf<UObject> InClass);
	void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity, const FFlecsId InCollectionId);

	template <Solid::TStaticClassConcept T>
	FORCEINLINE void RemoveCollectionFromEntity(const FFlecsEntityHandle& InEntity)
	{
		static_assert(TIsDerivedFrom<T, IFlecsCollectionInterface>::Value,
		              "T must implement IFlecsCollectionInterface");
		RemoveCollectionFromEntity(InEntity, T::StaticClass());
	}

	
	NO_DISCARD bool HasCollection(const FFlecsEntityHandle& InEntity, const FFlecsCollectionId& InCollectionId) const;

	NO_DISCARD bool IsCollectionRegistered(const FFlecsCollectionId& Id) const;

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

	void ApplyCollectionParametersToEntity(const FFlecsEntityHandle& InEntity,
		const FFlecsEntityHandle& InCollectionEntity,
		const FInstancedStruct& InParameters) const;

	void ApplyNamesToSubEntities(FFlecsCollectionDefinition& InDefinition) const;
	
	NO_DISCARD bool ClassImplementsCollectionInterface(const TSolidNotNull<const UClass*> InClass) const;

	UPROPERTY()
	TMap<FFlecsCollectionId, FFlecsEntityView> RegisteredCollections;

	// @TODO: make use of this
	// Recursion guard
	TSet<FFlecsCollectionId> InProgressCollections;

	UPROPERTY()
	FFlecsEntityHandle CollectionScopeEntity;
	
}; // class UFlecsCollectionWorldSubsystem
