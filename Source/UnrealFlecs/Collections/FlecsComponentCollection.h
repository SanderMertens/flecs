// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsComponentCollectionItem.h"
#include "FlecsComponentCollection.generated.h"

UENUM(BlueprintType)
enum class EFlecsCollectionApplicationType : uint8
{
	CurrentEntity,
	ChildEntity,
	SlotEntity,
}; // enum class EFlecsCollectionApplicationType

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsComponentCollection
{
	GENERATED_BODY()

public:
	FORCEINLINE FFlecsComponentCollection() = default;

	NO_DISCARD FORCEINLINE bool IsEmpty() const
	{
		return Components.IsEmpty();
	}

	NO_DISCARD FORCEINLINE bool HasComponent(const TSolidNotNull<const UScriptStruct*> ComponentStruct) const
	{
		const FString ComponentName = ComponentStruct->GetStructCPPName();
		
		return Components.ContainsByHash(GetTypeHash(ComponentName), ComponentStruct->GetStructCPPName());
	}

	NO_DISCARD FORCEINLINE bool HasComponent(const FString& ComponentName) const
	{
		return Components.ContainsByHash(GetTypeHash(ComponentName), ComponentName);
	}

	template <typename T>
	NO_DISCARD FORCEINLINE bool HasComponent() const
	{
		return HasComponent(FString(nameof(T).data()));
	}

	NO_DISCARD FORCEINLINE bool HasComponent(const FFlecsComponentCollectionItem& Component) const
	{
		return Components.Contains(Component);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flecs|Component Collection")
	EFlecsCollectionApplicationType ApplicationType = EFlecsCollectionApplicationType::CurrentEntity;

	UPROPERTY()
	TSet<FFlecsComponentCollectionItem> Components;
	
}; // struct FFlecsComponentCollection