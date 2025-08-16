// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "flecs.h"

#include "CoreMinimal.h"
#include "StructUtils/StructView.h"
#include "Types/SolidNotNull.h"
#include "FlecsComponentCollectionItem.generated.h"

USTRUCT()
struct UNREALFLECS_API FFlecsComponentCollectionItem
{
	GENERATED_BODY()

	NO_DISCARD FORCEINLINE friend uint32 GetTypeHash(const FFlecsComponentCollectionItem& InItem)
	{
		if (InItem.Second.IsEmpty())
		{
			return GetTypeHash(InItem.First);
		}
		else
		{
			return HashCombine(GetTypeHash(InItem.First), GetTypeHash(InItem.Second));
		}
	}

public:
	FORCEINLINE FFlecsComponentCollectionItem() = default;
	
	template <typename T>
	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const T* InComponentData = nullptr)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = FString(nameof(T).data());
		Item.Second.Empty();
		
		if (InComponentData)
		{
			Item.ComponentData.Append(reinterpret_cast<const uint8*>(InComponentData), sizeof(T));
		}
		
		return Item;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const TActual* InComponentData = nullptr)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = FString(nameof(TFirst).data());
		Item.Second = FString(nameof(TSecond).data());
		
		if (InComponentData)
		{
			Item.ComponentData.Append(reinterpret_cast<const uint8*>(InComponentData), sizeof(TActual));
		}
		
		return Item;
	}
	
	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const FString& InComponentName,
		const void* InComponentData = nullptr, const uint32 InComponentSize = 0)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = InComponentName;
		Item.Second.Empty();
		
		if (InComponentData && InComponentSize > 0)
		{
			Item.ComponentData.Append(reinterpret_cast<const uint8*>(InComponentData), InComponentSize);
		}
		
		return Item;
	}

	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const FConstStructView& InComponentData)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = InComponentData.GetScriptStruct()->GetStructCPPName();
		Item.Second = TEXT("");
		Item.ComponentData.Append(InComponentData.GetMemory(), InComponentData.GetScriptStruct()->GetStructureSize());
		return Item;
	}

	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const FConstStructView& FirstComponentData,
		const TSolidNotNull<const UScriptStruct*> SecondComponentType)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = FirstComponentData.GetScriptStruct()->GetStructCPPName();
		Item.Second = SecondComponentType->GetStructCPPName();
		Item.ComponentData.Append(FirstComponentData.GetMemory(), FirstComponentData.GetScriptStruct()->GetStructureSize());
		return Item;
	}

	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const TSolidNotNull<const UScriptStruct*> FirstComponentType,
		const FConstStructView& SecondComponentData)
	{
		// @TODO: should this be a check?
		checkf(FirstComponentType->GetStructureSize() > 1,
			TEXT("First component type must be valid when second component data is not empty."));
		
		FFlecsComponentCollectionItem Item;
		Item.First = FirstComponentType->GetStructCPPName();
		Item.Second = SecondComponentData.GetScriptStruct()->GetStructCPPName();
		Item.ComponentData.Append(SecondComponentData.GetMemory(), SecondComponentData.GetScriptStruct()->GetStructureSize());
		return Item;
	}

	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const TSolidNotNull<const UScriptStruct*> InComponentType)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = InComponentType->GetStructCPPName();
		Item.Second.Empty();
		return Item;
	}

	static NO_DISCARD FORCEINLINE FFlecsComponentCollectionItem Create(const TSolidNotNull<const UScriptStruct*> FirstComponentType,
		const TSolidNotNull<const UScriptStruct*> SecondComponentType)
	{
		FFlecsComponentCollectionItem Item;
		Item.First = FirstComponentType->GetStructCPPName();
		Item.Second = SecondComponentType->GetStructCPPName();
		return Item;
	}
	
	FORCEINLINE bool operator==(const FFlecsComponentCollectionItem& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	FORCEINLINE bool operator==(const FString& Other) const
	{
		return Second.IsEmpty() && First == Other;
	}

	FORCEINLINE bool operator!=(const FFlecsComponentCollectionItem& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE bool operator!=(const FString& Other) const
	{
		return !(*this == Other);
	}
	
	UPROPERTY()
	FString First;

	UPROPERTY()
	FString Second;

	UPROPERTY()
	TArray<uint8> ComponentData;
	
}; // struct FFlecsCollectionItem