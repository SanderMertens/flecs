// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsEntityRecord.h"
#include "Engine/DeveloperSettings.h"
#include "SolidMacros/Macros.h"
#include "FlecsDefaultEntitiesDeveloperSettings.generated.h"

namespace UnrealFlecs
{
	using EntityFunctionPtr = std::function<void(flecs::entity&)>;
} // namespace UnrealFlecs

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsDefaultMetaEntity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Flecs | Default Entities")
	FString EntityName;

	UPROPERTY(EditAnywhere, Category = "Flecs | Default Entities", meta = (ClampMin = "7000"))
	int32 EntityId = 7001;
	
	flecs::entity_t SetId = 0;

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsDefaultMetaEntity& Other) const
	{
		return EntityName == Other.EntityName && EntityId == Other.EntityId;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsDefaultMetaEntity& Other) const
	{
		return !(*this == Other);
	}

	FFlecsDefaultMetaEntity() = default;
	FFlecsDefaultMetaEntity(const FString& EntityName,
		const FFlecsEntityRecord& EntityRecord, const flecs::entity_t InEntityId)
		: EntityName(EntityName)
		, SetId(InEntityId)
	{
	}
	
}; // struct FFlecsEntityRecord

UCLASS(BlueprintType, Config = Flecs, DefaultConfig, Category = "Flecs",
	meta = (DisplayName = "Flecs Default Entities Developer Settings"))
class UNREALFLECS_API UFlecsDefaultEntitiesDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere, Config, Category = "Flecs | Default Entities")
	//TArray<FFlecsDefaultMetaEntity> DefaultEntities;

}; // class UFlecsDefaultEntitiesDeveloperSettings
