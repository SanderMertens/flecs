// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FlecsEntityHandleTypes.h"
#include "FlecsId.h"

#include "FlecsCommonHandle.generated.h"

class UFlecsWorld;


/**
 * @brief A common handle to a flecs entity. This is the base for all entity handles in Unreal-Flecs.
 */
USTRUCT(BlueprintInternalUseOnly)
struct UNREALFLECS_API FFlecsCommonHandle
{
	GENERATED_BODY()

	/**
	 * Get Input Id functions act as a way to convert various types to a FFlecsId,
	 * mainly used in combination with TFlecsEntityFunctionInputData concepts in @file FlecsEntityHandleTypes.h
	 */
	
	/**
	 * @brief Get the id from an id, this is a no-op but allows for consistent interface.
	 * @param InEntity The entity to get the id from.
	 * @param InId The id to get.
	 * @return The id.
	 */
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const FFlecsId InId)
	{
		return InId;
	}

	/**
	 * @brief Get the id from a struct type
	 * @param InEntity The entity to get the id from.
	 * @param StructType The struct type to get the id from.
	 * @return The id.
	 */
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const UScriptStruct* StructType)
	{
		return InEntity.ObtainComponentTypeStruct(StructType);
	}

	/**
	 * @brief Get the id from an enum type
	 * @param InEntity The entity to get the id from.
	 * @param EnumType The enum type to get the id from.
	 * @return The id.
	 */
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const UEnum* EnumType)
	{
		return InEntity.ObtainComponentTypeEnum(EnumType);
	}
	
	/**
	 * @brief Get the id from a gameplay tag
	 * @param InEntity The entity to get the id from.
	 * @param InTag The tag to get the id from.
	 * @return The id.
	 */
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, const FGameplayTag& InTag)
	{
		return InEntity.GetTagEntity(InTag);
	}
	
	/**
	 * @brief Get the id from a class type
	 * @param InEntity The entity to get the id from.
	 * @param InClass The class type to get the id from.
	 * @return The id.
	 */
	static FORCEINLINE FFlecsId GetInputId(const FFlecsCommonHandle& InEntity, UClass* InClass)
	{
		return InEntity.ObtainTypeClass(InClass);
	}

	NO_DISCARD SOLID_INLINE friend uint32 GetTypeHash(const FFlecsCommonHandle& InEntity)
	{
		return GetTypeHash(InEntity.GetFlecsId());
	}

	/**
	 * @brief Check if entity is valid.
	 * An entity is valid if:
	 * - its id is not 0
	 * - the id contains a valid bit pattern for an entity
	 * - the entity is alive (see is_alive())
	 * 
	 * @return True if the entity is valid, false otherwise.
	 * @see ecs_is_valid()
	 */
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

	/**
	 * @brief Check if entity is valid.
	 * An entity is valid if:
	 * - its id is not 0
	 * - the id contains a valid bit pattern for an entity
	 * - the entity is alive (see is_alive())
	 * 
	 * @return True if the entity is valid, false otherwise.
	 * @see ecs_is_valid()
	 */
	NO_DISCARD SOLID_INLINE bool IsValid() const
	{
		return GetEntity().is_valid();
	}

	/**
	 * @brief Check if entity is alive.
	 * @return True if the entity is alive, false otherwise.
	 * @see ecs_is_alive()
	 */
	NO_DISCARD SOLID_INLINE bool IsAlive() const
	{
		return GetEntity().is_alive();
	}

	/**
	 * @brief Check if entity is valid.
	 * An entity is valid if:
	 * - its id is not 0
	 * - the id contains a valid bit pattern for an entity
	 * - the entity is alive (see is_alive())
	 * 
	 * @return True if the entity is valid, false otherwise.
	 * @see ecs_is_valid()
	 */
	SOLID_INLINE operator bool() const
	{
		return GetEntity().operator bool();
	}

	/**
	 * @brief Set the entity handle from a flecs::entity.
	 * @param InEntity The entity to set.
	 */
	SOLID_INLINE void SetEntity(const flecs::entity& InEntity)
	{
		*this = FFlecsCommonHandle(InEntity);
	}

	// Disabled because this can be confused with the flecs::entity version above, as well as this not having world context.
	#if 0
	/**
	 * @brief Set the entity handle.
	 * @param InEntity The entity to set.
	 */
	SOLID_INLINE void SetEntity(const FFlecsId InEntity)
	{
		*this = FFlecsCommonHandle(InEntity);
	}
	#endif // 0
	
	SOLID_INLINE operator FFlecsId() const
	{
		return FFlecsId(GetEntity());
	}

	/**
	 * @brief Get the native flecs id (flecs::id), this will still contain a world context.
	 * @return The native flecs id.
	 */
	NO_DISCARD SOLID_INLINE flecs::id GetNativeId() const
	{
		return Entity;
	}
	
	/**
	 * @brief Get the raw id (flecs::id_t), this is just the integer id without any world context.
	 * @return The raw id.
	 */
	NO_DISCARD SOLID_INLINE flecs::id_t GetRawId() const
	{
		return Entity.raw_id();
	}

	/**
	 * @brief Get the flecs entity (flecs::entity), this will still contain a world context.
	 * @return The flecs entity handle.
	 */
	NO_DISCARD SOLID_INLINE flecs::entity GetEntity() const
	{
		return flecs::entity(Entity.world(), Entity);
	}

	/**
	 * @brief Get the flecs entity view (flecs::entity_view), this will still contain a world context.
	 * @return The flecs entity view handle.
	 */
	NO_DISCARD SOLID_INLINE flecs::entity_view GetEntityView() const
	{
		return flecs::entity_view(Entity.world(), Entity);
	}
	
	SOLID_INLINE operator flecs::entity() const
	{
		return GetEntity();
	}

	SOLID_INLINE operator flecs::id_t() const
	{
		return GetEntity();
	}

	/**
	 * @brief Get the native flecs world, not necessarily an UnrealFlecs world.
	 * @return The native flecs world.
	 */
	NO_DISCARD flecs::world GetNativeFlecsWorld() const;
	
	/**
	 * @brief Get the UnrealFlecs world, will assert if the world is not an UnrealFlecs world.
	 * @return The UnrealFlecs world.
	 */
	NO_DISCARD UFlecsWorld* GetFlecsWorld() const;

	NO_DISCARD TSolidNotNull<UFlecsWorld*> GetFlecsWorldChecked() const;
	
	/**
	 * @brief Check if the world is an UnrealFlecs world.
	 * @return True if the world is an UnrealFlecs world, false otherwise.
	 */
	NO_DISCARD bool IsUnrealFlecsWorld() const;
	
	/**
	 * @brief Get the outer world (UWorld) of the flecs world,
		will assert if the world is not valid or if the flecs world is not an UnrealFlecs world.
	 * @return The outer world.
	 */
	NO_DISCARD TSolidNotNull<UWorld*> GetOuterWorld() const;
	
	/**
	 * @brief Get the name of the flecs world, World must be an UnrealFlecs world.
	 * @return The name of the flecs world.
	 */
	NO_DISCARD FString GetWorldName() const;

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const
	{
		//return THandleType(GetNativeFlecsWorld(), ObtainComponentTypeStruct(StructType));
		return ObtainComponentTypeStruct(StructType).ToHandle<THandleType>(GetNativeFlecsWorld());
	}
	
	NO_DISCARD FFlecsId ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const;

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const
	{
		return ObtainComponentTypeEnum(EnumType).ToHandle<THandleType>(GetNativeFlecsWorld());
	}
	
	NO_DISCARD FFlecsId ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const;

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType ObtainTypeClass(const TSolidNotNull<UClass*> ClassType) const
	{
		return ObtainTypeClass(ClassType).ToHandle<THandleType>(GetNativeFlecsWorld());
	}

	NO_DISCARD FFlecsId ObtainTypeClass(const TSolidNotNull<UClass*> ClassType) const;

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandleType>
	NO_DISCARD SOLID_INLINE THandleType GetTagEntity(const FGameplayTag& InTag) const
	{
		solid_checkf(InTag.IsValid(), TEXT("Invalid GameplayTag passed to GetTagEntity!"));
		return GetTagEntity(InTag).ToHandle<THandleType>(GetNativeFlecsWorld());
	}

	/**
	 * @brief Convert a gameplay tag to a flecs entity.
	 * @param InTag The gameplay tag to convert.
	 * @return The flecs entity representing the tag.
	 */
	NO_DISCARD FFlecsId GetTagEntity(const FGameplayTag& InTag) const;

	/**
	 * @brief Check if the entity handle is in a stage context.
	 * @return True if the entity handle is in a stage context, false otherwise.
	 */
	NO_DISCARD SOLID_INLINE bool IsInStage() const
	{
		return GetNativeFlecsWorld().is_stage();
	}

	/**
	 * @brief Get the flecs id (FFlecsId) of the entity, this is just a wrapper around GetRawId().
	 * @return The flecs id (FFlecsId).
	 */
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

	/**
	 * @brief Converts the id of the entity to a string.
	 * @return The string representation of the entity id.
	 */
	NO_DISCARD SOLID_INLINE FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

	/**
	 * @brief Convert this handle to another handle type.
	 * @tparam THandle The handle type to convert to. Must satisfy TFlecsEntityHandleTypeConcept.
	 * @return The converted handle.
	 */
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle ToHandle() const
	{
		return THandle(GetNativeFlecsWorld(), GetFlecsId());
	}

protected:
	flecs::id Entity;
	
}; // struct FFlecsCommonHandle

template <typename TInherited>
struct UNREALFLECS_API TTypedFlecsCommonHandle : public FFlecsCommonHandle
{
	using InheritedType = TInherited;
	using SelfType = InheritedType;
	
}; // struct TTypedFlecsCommonHandle
