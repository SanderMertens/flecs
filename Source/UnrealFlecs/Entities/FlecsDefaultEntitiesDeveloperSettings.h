// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DeveloperSettings.h"

#include "SolidMacros/Macros.h"

#include "FlecsEntityRecord.h"

#include "FlecsDefaultEntitiesDeveloperSettings.generated.h"

namespace Unreal::Flecs
{
	// Not using FFlecsEntityHandle as we can't guarantee the entity is in the context of a UFlecsWorld
	using EntityFunctionPtr = std::function<void(flecs::entity)>;
} // namespace Unreal::Flecs

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsDefaultMetaEntity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Default Entities")
	FString EntityName;

	UPROPERTY(EditAnywhere, Category = "Default Entities", meta = (ClampMin = "7000"))
	uint32 EntityId = 7001;
	
	flecs::entity_t SetId = 0;

	NO_DISCARD FORCEINLINE bool operator==(const FFlecsDefaultMetaEntity& Other) const
	{
		return EntityName == Other.EntityName && EntityId == Other.EntityId;
	}

	NO_DISCARD FORCEINLINE bool operator!=(const FFlecsDefaultMetaEntity& Other) const
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
class UFlecsDefaultEntitiesDeveloperSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//@TODO: Implement this or add an equivalent feature.
	//UPROPERTY(EditAnywhere, Config, Category = "Default Entities")
	//TArray<FFlecsDefaultMetaEntity> DefaultEntities;

}; // class UFlecsDefaultEntitiesDeveloperSettings
