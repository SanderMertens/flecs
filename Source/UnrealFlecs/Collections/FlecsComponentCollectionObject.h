// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "FlecsComponentCollectionObject.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class UNREALFLECS_API UFlecsComponentCollectionObject : public UObject
{
	GENERATED_BODY()

public:
	template <typename T>
	FORCEINLINE static NO_DISCARD FString GetCollectionTypeName()
	{
		static_assert(std::is_base_of<UFlecsComponentCollectionObject, T>::value,
			"Type is not a component collection object");
		return T::StaticClass()->template GetDefaultObject<T>()->GetCollectionName();
	}

	FORCEINLINE static NO_DISCARD FString GetCollectionTypeName(const UClass* CollectionClass)
	{
		solid_checkf(CollectionClass, TEXT("Collection class is not valid"));
		solid_checkf(CollectionClass->IsChildOf<UFlecsComponentCollectionObject>(),
			TEXT("Collection class is not a component collection object"));
		return CollectionClass->GetDefaultObject<UFlecsComponentCollectionObject>()->GetCollectionName();
	}
	
	UFlecsComponentCollectionObject();
	UFlecsComponentCollectionObject(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Flecs Component Collection")
	FString CollectionName;

	FString GetCollectionName() const;

	FFlecsEntityHandle ObtainCollectionEntity(UFlecsWorld* InFlecsWorld);

	/* Applies to the Prefab Entity */
	UFUNCTION(BlueprintNativeEvent, Category = "Flecs Component Collection")
	void ApplyCollectionToEntity(FFlecsEntityHandle& Entity);

	void ApplyCollection_Internal(FFlecsEntityHandle& Entity, UFlecsWorld* InFlecsWorld);

	UFUNCTION(BlueprintCallable, Category = "Flecs Component Collection")
	UFlecsWorld* GetFlecsWorld() const;

	virtual UWorld* GetWorld() const override final;
	
protected:

	UPROPERTY()
	FFlecsEntityHandle CollectionEntity = FFlecsEntityHandle::GetNullHandle();

	UPROPERTY()
	mutable TObjectPtr<UFlecsWorld> FlecsWorld;

}; // class UFlecsComponentCollectionObject
