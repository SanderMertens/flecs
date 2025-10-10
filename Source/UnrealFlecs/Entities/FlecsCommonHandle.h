// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlecsEntityHandleTypes.h"
#include "FlecsId.h"

#include "FlecsCommonHandle.generated.h"

class UFlecsWorld;

USTRUCT(BlueprintInternalUseOnly)
struct UNREALFLECS_API FFlecsCommonHandle
{
	GENERATED_BODY()
	
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const FFlecsId InId)
	{
		return InId;
	}

	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const UScriptStruct* StructType)
	{
		return InEntity.ObtainComponentTypeStruct(StructType);
	}

	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const UEnum* EnumType)
	{
		return InEntity.ObtainComponentTypeEnum(EnumType);
	}

	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const FGameplayTag& InTag)
	{
		return InEntity.GetTagEntity(InTag);
	}

	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, UClass* InClass)
	{
		return InEntity.ObtainTypeClass(InClass);
	}

	NO_DISCARD SOLID_INLINE friend uint32 GetTypeHash(const FFlecsCommonHandle& InEntity)
	{
		return GetTypeHash(InEntity.GetFlecsId());
	}

	NO_DISCARD FORCEINLINE friend bool IsValid(const FFlecsCommonHandle& Test)
	{
		return Test.IsValid();
	}

public:
	FFlecsCommonHandle() = default;
	
	SOLID_INLINE explicit FFlecsCommonHandle(const flecs::entity& InEntity)
		: Entity(InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsCommonHandle(const FFlecsId InEntity)
	{
		Entity = flecs::entity(InEntity);
	}

	SOLID_INLINE FFlecsCommonHandle(const flecs::world& InWorld, const FFlecsId InEntity)
	{
		Entity = flecs::entity(InWorld, InEntity);
	}

	FFlecsCommonHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity);
	FFlecsCommonHandle(const flecs::world_t* InWorld, const FFlecsId InEntity);
	
	SOLID_INLINE FFlecsCommonHandle(const flecs::entity_view& View)
		: Entity(View)
	{
	}

	NO_DISCARD SOLID_INLINE bool IsValid() const
	{
		return GetEntity().is_valid();
	}
	
	NO_DISCARD SOLID_INLINE bool IsAlive() const
	{
		return GetEntity().is_alive();
	}

	SOLID_INLINE operator bool() const
	{
		return GetEntity().operator bool();
	}

	SOLID_INLINE void SetEntity(const flecs::entity& InEntity)
	{
		*this = FFlecsCommonHandle(InEntity);
	}
	
	SOLID_INLINE void SetEntity(const FFlecsId InEntity)
	{
		*this = FFlecsCommonHandle(InEntity);
	}

	SOLID_INLINE operator FFlecsId() const
	{
		return FFlecsId(GetEntity());
	}

	NO_DISCARD SOLID_INLINE flecs::id GetId() const
	{
		return Entity;
	}

	NO_DISCARD SOLID_INLINE flecs::id_t GetRawId() const
	{
		return Entity.raw_id();
	}

	NO_DISCARD SOLID_INLINE flecs::entity GetEntity() const
	{
		return flecs::entity(GetFlecsWorld_Internal(), Entity);
	}

	NO_DISCARD SOLID_INLINE flecs::entity_view GetEntityView() const
	{
		return flecs::entity_view(GetFlecsWorld_Internal(), Entity);
	}

	SOLID_INLINE operator flecs::entity() const
	{
		return GetEntity();
	}
	
	SOLID_INLINE operator flecs::id_t() const
	{
		return GetEntity();
	}

	NO_DISCARD flecs::world GetNativeFlecsWorld() const;
	
	NO_DISCARD TSolidNotNull<UFlecsWorld*> GetFlecsWorld() const;
	NO_DISCARD bool IsUnrealFlecsWorld() const;
	
	NO_DISCARD TSolidNotNull<UWorld*> GetOuterWorld() const;
	NO_DISCARD FString GetWorldName() const;

	// @TODO: be careful with these functions as they may create new component types in the Flecs world!
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const
	{
		return THandleType(GetFlecsWorld_Internal(), ObtainComponentTypeStruct(StructType));
	}
	
	NO_DISCARD FFlecsId ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const;

	// @TODO: be careful with these functions as they may create new component types in the Flecs world!
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const
	{
		return THandleType(GetFlecsWorld_Internal(), ObtainComponentTypeEnum(EnumType));
	}
	
	NO_DISCARD FFlecsId ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const;

	// @TODO: be careful with these functions as they may create new component types in the Flecs world!
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainTypeClass(const TSolidNotNull<UClass*> ClassType) const
	{
		return THandleType(GetFlecsWorld_Internal(), ObtainTypeClass(ClassType));
	}

	NO_DISCARD FFlecsId ObtainTypeClass(const TSolidNotNull<UClass*> ClassType) const;

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType GetTagEntity(const FGameplayTag& InTag) const
	{
		return THandleType(GetFlecsWorld_Internal(), GetTagEntity(InTag));
	}

	NO_DISCARD FFlecsId GetTagEntity(const FGameplayTag& InTag) const;

	NO_DISCARD SOLID_INLINE bool IsInStage() const
	{
		return GetEntity().world().is_stage();
	}

	NO_DISCARD SOLID_INLINE FFlecsId GetFlecsId() const
	{
		return FFlecsId(GetRawId());
	}
	
	NO_DISCARD SOLID_INLINE uint32 GetGeneration() const
	{
		return GetFlecsId().GetGeneration();
	}
	
	NO_DISCARD SOLID_INLINE uint32 GetVersion() const
	{
		return GetGeneration();
	}

	NO_DISCARD SOLID_INLINE FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

protected:
	flecs::id Entity;

	NO_DISCARD SOLID_INLINE flecs::world GetFlecsWorld_Internal() const
	{
		return Entity.world();
	}
	
}; // struct FFlecsCommonHandle

template <typename TInherited>
struct UNREALFLECS_API TTypedFlecsCommonHandle : public FFlecsCommonHandle
{
	using InheritedType = TInherited;
	using SelfType = InheritedType;
	
}; // struct TTypedFlecsCommonHandle
