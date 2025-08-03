// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/StructView.h"
#include "Types/SolidNotNull.h"
#include "FlecsComponentCollectionItem.generated.h"

USTRUCT()
struct alignas(16) FFlecsComponentCollectionItem
{
	GENERATED_BODY()
	
	NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsComponentCollectionItem& InEntity)
	{
		return GetTypeHash(InEntity.ComponentTypeName);
	}
	
public:
	FORCEINLINE FFlecsComponentCollectionItem() = default;
	
	template <typename T>
	FORCEINLINE explicit FFlecsComponentCollectionItem(const T& InComponentData)
		: ComponentTypeName(nameof(T).data())
	{
		const uint8* DataPtr = reinterpret_cast<const uint8*>(&InComponentData);
		ComponentData = TArray<uint8>(DataPtr, sizeof(T));
	}

	FORCEINLINE explicit FFlecsComponentCollectionItem(const FString& InComponentTypeName, const TArray<uint8>& InComponentData)
		: ComponentTypeName(InComponentTypeName), ComponentData(InComponentData)
	{
	}

	FORCEINLINE explicit FFlecsComponentCollectionItem(const FConstStructView& ComponentDataView)
		: ComponentTypeName(ComponentDataView.GetScriptStruct()->GetStructCPPName())
	{
		const uint8* DataPtr = ComponentDataView.GetMemory();
		ComponentData = TArray<uint8>(DataPtr, ComponentDataView.GetScriptStruct()->GetStructureSize());
	}

	FORCEINLINE explicit FFlecsComponentCollectionItem(const TSolidNotNull<UScriptStruct*> ComponentType) :
		ComponentTypeName(ComponentType->GetStructCPPName())
	{
		ComponentData = TArray<uint8>();
	}

	template <typename T>
	FORCEINLINE explicit FFlecsComponentCollectionItem()
		: ComponentTypeName(nameof(T).data())
	{
		ComponentData = TArray<uint8>();
	}

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsComponentCollectionItem& Other) const
	{
		return ComponentTypeName == Other.ComponentTypeName;
	}

	NO_DISCARD FORCEINLINE bool operator==(const FString& OtherComponentTypeName) const
	{
		return ComponentTypeName == OtherComponentTypeName;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsComponentCollectionItem& Other) const
	{
		return !(*this == Other);
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FString& OtherComponentTypeName) const
	{
		return !(*this == OtherComponentTypeName);
	}

	UPROPERTY()
	FString ComponentTypeName;

	UPROPERTY()
	TArray<uint8> ComponentData;
	
}; // struct FFlecsComponentCollectionItem