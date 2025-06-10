// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <functional>

#include "flecs.h"

#include "CoreMinimal.h"
#include "FlecsArchetype.h"
#include "FlecsId.h"
#include "GameplayTagContainer.h"
#include "General/FlecsStringConverters.h"
#include "SolidMacros/Macros.h"
#include "StructUtils/InstancedStruct.h"
#include "Types/SolidEnumSelector.h"
#include "Types/SolidNotNull.h"
#include "FlecsEntityHandle.generated.h"

class UFlecsWorld;

/**
 * @struct FFlecsEntityHandle
 *
 * A handle for managing flecs entities in Unreal Engine with added blueprint support.
 * The structure provides several utility functions to interact with flecs entities,
 * including validation, component addition/removal, and direct data access.
 * This must be used with a valid `UFlecsWorld` instance to function correctly.
 */
USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct alignas(8) UNREALFLECS_API FFlecsEntityHandle
{
	GENERATED_BODY()

	NO_DISCARD FORCEINLINE friend bool IsValid(const FFlecsEntityHandle& Test)
	{
		return Test.IsValid();
	}

	NO_DISCARD SOLID_INLINE friend uint32 GetTypeHash(const FFlecsEntityHandle& InEntity)
	{
		return GetTypeHash(InEntity.GetFlecsId());
	}

	NO_DISCARD SOLID_INLINE static FFlecsEntityHandle GetNullHandle()
	{
		return FFlecsEntityHandle(flecs::entity::null());
	}

	NO_DISCARD static FFlecsEntityHandle GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	FFlecsEntityHandle() = default;
	
	SOLID_INLINE FFlecsEntityHandle(const flecs::entity& InEntity) : Entity(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityHandle(const FFlecsId InEntity)
	{
		Entity = flecs::entity(InEntity);
	}

	SOLID_INLINE FFlecsEntityHandle(flecs::world& InWorld, const FFlecsId InEntity)
	{
		Entity = flecs::entity(InWorld, InEntity);
	}

	FFlecsEntityHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity);

	FFlecsEntityHandle(const flecs::world_t* InWorld, const FFlecsId InEntity);
	
	NO_DISCARD SOLID_INLINE flecs::entity GetEntity() const
	{
		return Entity;
	}

	SOLID_INLINE void SetEntity(const flecs::entity& InEntity)
	{
		*this = FFlecsEntityHandle(InEntity);
	}
	
	SOLID_INLINE void SetEntity(const FFlecsId InEntity)
	{
		*this = FFlecsEntityHandle(InEntity);
	}
	
	SOLID_INLINE operator flecs::entity() const
	{
		return GetEntity();
	}
	
	SOLID_INLINE operator flecs::id_t() const
	{
		return GetEntity();
	}
	
	SOLID_INLINE operator flecs::entity_view() const
	{
		return GetEntity().view();
	}

	SOLID_INLINE operator FFlecsId() const
	{
		return FFlecsId(GetEntity());
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

	NO_DISCARD SOLID_INLINE FFlecsId GetFlecsId() const
	{
		return FFlecsId(GetEntity());
	}
	
	NO_DISCARD SOLID_INLINE uint32 GetGeneration() const
	{
		return GetFlecsId().GetGeneration();
	}
	
	NO_DISCARD SOLID_INLINE uint32 GetVersion() const
	{
		return GetGeneration();
	}
	
	NO_DISCARD TSolidNotNull<UFlecsWorld*> GetFlecsWorld() const;
	NO_DISCARD TSolidNotNull<UWorld*> GetOuterWorld() const;
	NO_DISCARD FString GetWorldName() const;
	
	NO_DISCARD SOLID_INLINE FFlecsArchetype GetType() const
	{
		return FFlecsArchetype(GetEntity().type());
	}

	NO_DISCARD SOLID_INLINE bool Has(const FFlecsId InEntity) const
	{
		return GetEntity().has(InEntity);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Has() const
	{
		return GetEntity().has<T>();
	}

	NO_DISCARD SOLID_INLINE bool Has(const UScriptStruct* StructType) const
	{
		return Has(ObtainComponentTypeStruct(StructType));
	}

	NO_DISCARD SOLID_INLINE bool Has(const FGameplayTag& InTag) const
	{
		return Has(GetTagEntity(InTag));
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType) const
	{
		return HasPair(ObtainComponentTypeEnum(EnumType), flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(EnumType, InValue);
		
		return HasPair(EnumEntity, EnumConstant);
	}

	NO_DISCARD SOLID_INLINE bool Has(const FSolidEnumSelector& EnumSelector) const
	{
		return Has(EnumSelector.Class, EnumSelector.Value);
	}

	SOLID_INLINE const FFlecsEntityHandle& Add(const FFlecsId InEntity) const
	{
		GetEntity().add(InEntity);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Add(const UScriptStruct* StructType) const
	{
		Add(ObtainComponentTypeStruct(StructType));
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Add(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);

		const FFlecsId ValueEntity = ObtainEnumConstant(EnumType, InValue);
		solid_check(ValueEntity.IsValid());
		
		AddPair(EnumEntity, ValueEntity);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Add(const FGameplayTag& InTag) const
	{
		Add(GetTagEntity(InTag));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Add(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			Add(Tag);
		}

		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Add() const
	{
		GetEntity().add<T>();
		return *this;
	}

	template <typename T
		UE_REQUIRES (std::is_enum<T>::value)>
	SOLID_INLINE const FFlecsEntityHandle& Add(const T InValue) const
	{
		GetEntity().add<T>(InValue);
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Remove(const FFlecsId InEntity) const
	{
		GetEntity().remove(InEntity);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Remove(const UScriptStruct* StructType) const
	{
		Remove(ObtainComponentTypeStruct(StructType));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Remove(const FGameplayTag& InTag) const
	{
		Remove(GetTagEntity(InTag));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Remove(const UEnum* EnumType) const
	{
		RemovePair(ObtainComponentTypeEnum(EnumType), flecs::Wildcard);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Remove(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(EnumType, InValue);
		
		RemovePair(EnumEntity, EnumConstant);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Remove(const FGameplayTagContainer& InTags, const bool bMustHaveAll = false) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			if (!bMustHaveAll)
			{
				if (!Has(Tag))
				{
					continue;
				}
			}
			
			Remove(Tag);
		}

		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Remove() const
	{
		GetEntity().remove<T>();
		return *this;
	}

	template <typename First, typename Second>
	SOLID_INLINE const FFlecsEntityHandle& RemovePair() const
	{
		GetEntity().remove<First, Second>();
		return *this;
	}

	template <typename First>
	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FFlecsId InSecond) const
	{
		GetEntity().remove<First>(InSecond);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Set(const T& InValue) const
	{
		GetEntity().set<T>(InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Set(const FFlecsId InId, const void* InValue) const
	{
		GetEntity().set_ptr(InId, InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Set(const FFlecsId InId, const uint32 InSize, const void* InValue) const
	{
		GetEntity().set_ptr(InId, InSize, InValue);
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Set(const UScriptStruct* StructType, const void* InValue) const
	{
		Set(ObtainComponentTypeStruct(StructType), InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Set(const FInstancedStruct& InValue) const
	{
		Set(ObtainComponentTypeStruct(InValue.GetScriptStruct()), InValue.GetMemory());
		return *this;
	}
	
	template <typename T>
	NO_DISCARD SOLID_INLINE T Get() const
	{
		check(Has<T>());
		return *GetEntity().get<T>();
	}

	NO_DISCARD SOLID_INLINE void* GetPtr(const FFlecsId InEntity)
	{
		return GetEntity().get_mut(InEntity);
	}

	NO_DISCARD SOLID_INLINE const void* GetPtr(const FFlecsId InEntity) const
	{
		return GetEntity().get(InEntity);
	}
	
	template <typename T>
	NO_DISCARD SOLID_INLINE T* GetPtr()
	{
		return GetEntity().get_mut<T>();
	}
	
	template <typename T>
	NO_DISCARD SOLID_INLINE const T* GetPtr() const
	{
		return GetEntity().get<T>();
	}

	NO_DISCARD SOLID_INLINE void* GetPtr(const UScriptStruct* StructType)
	{
		return GetPtr(ObtainComponentTypeStruct(StructType));
	}

	NO_DISCARD SOLID_INLINE const void* GetPtr(const UScriptStruct* StructType) const
	{
		return GetPtr(ObtainComponentTypeStruct(StructType));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T& GetRef()
	{
		return *GetEntity().get_ref<T>().get();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE const T& GetRef() const
	{
		return *GetEntity().get_ref<T>().get();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE flecs::ref<T> GetFlecsRef() const
	{
		return GetEntity().get_ref<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE flecs::ref<T> GetFlecsRef()
	{
		return GetEntity().get_ref<T>();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Has()
	{
		return GetEntity().has<T>();
	}

	SOLID_INLINE void Clear() const
	{
		GetEntity().clear();
	}

	SOLID_INLINE const FFlecsEntityHandle& Enable() const
	{
		GetEntity().enable();
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Disable() const
	{
		GetEntity().disable();
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Enable() const
	{
		GetEntity().enable<T>();
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Enable(const FFlecsId InEntity) const
	{
		GetEntity().enable(InEntity);
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Enable(const UScriptStruct* StructType) const
	{
		Enable(ObtainComponentTypeStruct(StructType));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Enable(const FGameplayTag& InTag) const
	{
		Enable(GetTagEntity(InTag));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Disable(const FFlecsId InEntity) const
	{
		GetEntity().disable(InEntity);
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Disable(const UScriptStruct* StructType) const
	{
		Disable(ObtainComponentTypeStruct(StructType));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Disable(const FGameplayTag& InTag) const
	{
		Disable(GetTagEntity(InTag));
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Disable() const
	{
		GetEntity().disable<T>();
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Toggle() const
	{
		IsEnabled() ? Disable() : Enable();
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& Toggle() const
	{
		IsEnabled<T>() ? Disable<T>() : Enable<T>();
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Toggle(const FFlecsId InEntity) const
	{
		GetEntity().enable(InEntity, !IsEnabled(InEntity));
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& Toggle(const UScriptStruct* StructType) const
	{
		Toggle(ObtainComponentTypeStruct(StructType));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& Toggle(const FGameplayTag& InTag) const
	{
		Toggle(GetTagEntity(InTag));
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntity().enabled();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntity().enabled<T>();
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled(const FFlecsId InEntity) const
	{
		return GetEntity().enabled(InEntity);
	}
	
	NO_DISCARD SOLID_INLINE bool IsEnabled(const UScriptStruct* StructType) const
	{
		return IsEnabled(ObtainComponentTypeStruct(StructType));
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled(const FGameplayTag& InTag) const
	{
		return IsEnabled(GetTagEntity(InTag));
	}

	SOLID_INLINE void Destroy() const
	{
		GetEntity().destruct();
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Clone(const bool bCloneValue = true,
	                                                 const FFlecsId DestinationId = 0) const
	{
		return GetEntity().clone(bCloneValue, DestinationId);
	}

	SOLID_INLINE const FFlecsEntityHandle& SetName(const FString& InName) const
	{
		GetEntity().set_name(Unreal::Flecs::ToCString(InName));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetName() const
	{
		return FString(GetEntity().name().c_str(), GetEntity().name().length());
	}
	
	NO_DISCARD SOLID_INLINE bool HasName() const
	{
		return HasPair<flecs::Identifier>(flecs::Name);
	}

	NO_DISCARD SOLID_INLINE FString GetSymbol() const
	{
		return FString(GetEntity().symbol().c_str(), GetEntity().symbol().length());
	}
	
	NO_DISCARD SOLID_INLINE bool HasSymbol() const
	{
		return HasPair<flecs::Identifier>(flecs::Symbol);
	}

	SOLID_INLINE const FFlecsEntityHandle& SetDocBrief(const FString& InDocBrief) const
	{
		GetEntity().set_doc_brief(Unreal::Flecs::ToCString(InDocBrief));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocBrief() const
	{
		return GetEntity().doc_brief();
	}

	SOLID_INLINE const FFlecsEntityHandle& SetDocColor(const FString& Link) const
	{
		GetEntity().set_doc_color(Unreal::Flecs::ToCString(Link));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocColor() const
	{
		return GetEntity().doc_color();
	}

	SOLID_INLINE const FFlecsEntityHandle& SetDocName(const FString& InDocName) const
	{
		GetEntity().set_doc_name(Unreal::Flecs::ToCString(InDocName));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocName() const
	{
		return GetEntity().doc_name();
	}

	SOLID_INLINE const FFlecsEntityHandle& SetDocLink(const FString& InDocLink) const
	{
		GetEntity().set_doc_link(Unreal::Flecs::ToCString(InDocLink));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocLink() const
	{
		return GetEntity().doc_link();
	}

	SOLID_INLINE const FFlecsEntityHandle& SetDocDetails(const FString& InDocDetails) const
	{
		GetEntity().set_doc_detail(Unreal::Flecs::ToCString(InDocDetails));
		return *this;
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocDetails() const
	{
		return GetEntity().doc_detail();
	}
	
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetParent() const
	{
		return GetEntity().parent();
	}

	NO_DISCARD SOLID_INLINE bool HasParent() const
	{
		return HasPair(flecs::ChildOf, flecs::Wildcard);
	}

	SOLID_INLINE const FFlecsEntityHandle& SetParent(const FFlecsId InParent) const
	{
		GetEntity().child_of(InParent);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetParent(const FFlecsId InParent, const bool bIsA) const
	{
		SetParent(InParent);

		if (bIsA)
		{
			GetEntity().is_a(InParent);
		}
		
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool IsPrefab() const
	{
		return Has(flecs::Prefab);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool DoesOwn() const
	{
		return GetEntity().owns<T>();
	}
	
	NO_DISCARD SOLID_INLINE bool DoesOwn(const FFlecsId InEntity) const
	{
		return GetEntity().owns(InEntity);
	}

	NO_DISCARD SOLID_INLINE bool DoesOwn(const UScriptStruct* StructType) const
	{
		return DoesOwn(ObtainComponentTypeStruct(StructType));
	}

	template <typename First, typename Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair() const
	{
		return GetEntity().owns<First, Second>();
	}

	template <typename First>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const FFlecsId InSecond) const
	{
		return GetEntity().owns<First>(InSecond);
	}

	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		return GetEntity().owns(InFirst, InSecond);
	}

	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const UScriptStruct* First, const UScriptStruct* Second) const
	{
		return DoesOwnPair(ObtainComponentTypeStruct(First), ObtainComponentTypeStruct(Second));
	}
	
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const UScriptStruct* First, const FFlecsId Second) const
	{
		return DoesOwnPair(ObtainComponentTypeStruct(First), Second);
	}

	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const FFlecsId First, const UScriptStruct* Second) const
	{
		return DoesOwnPair(First, ObtainComponentTypeStruct(Second));
	}

	NO_DISCARD SOLID_INLINE bool IsComponent() const
	{
		return Has<flecs::Component>();
	}

	NO_DISCARD SOLID_INLINE bool IsEnum() const
	{
		return Has<flecs::Enum>();
	}

	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent() const
	{
		solid_checkf(IsComponent(), TEXT("Entity is not a component"));
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent_Unsafe() const
	{
		return flecs::untyped_component(GetFlecsWorld_Internal(), GetEntity());
	}

	template <typename T>
	SOLID_INLINE void Emit() const
	{
		GetEntity().emit<T>();
	}

	template <typename T>
	SOLID_INLINE void Emit(const T& InValue) const
	{
		GetEntity().emit<T>(InValue);
	}

	SOLID_INLINE void Emit(const FFlecsId InEntity) const
	{
		GetEntity().emit(InEntity);
	}

	template <typename T>
	SOLID_INLINE void Enqueue() const
	{
		GetEntity().enqueue<T>();
	}

	template <typename T>
	SOLID_INLINE void Enqueue(const T& InValue) const
	{
		GetEntity().enqueue<T>(InValue);
	}

	SOLID_INLINE void Enqueue(const FFlecsId InEntity) const
	{
		GetEntity().enqueue(InEntity);
	}

	SOLID_INLINE void Enqueue(const UScriptStruct* StructType) const
	{
		Enqueue(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Enqueue(const FGameplayTag& InTag) const
	{
		Enqueue(GetTagEntity(InTag));
	}

	template <typename TEvent, typename FunctionType>
	SOLID_INLINE const FFlecsEntityHandle& Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe<TEvent>(std::forward<FunctionType>(InFunction));
		return *this;
	}

	template <typename FunctionType>
	SOLID_INLINE const FFlecsEntityHandle& Observe(const FFlecsId InEntity, FunctionType&& InFunction) const
	{
		GetEntity().observe(InEntity, std::forward<FunctionType>(InFunction));
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool operator==(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() == Other.GetEntity();
	}
	
	NO_DISCARD SOLID_INLINE bool operator!=(const FFlecsEntityHandle& Other) const
	{
		return GetEntity() != Other.GetEntity();
	}
	
	NO_DISCARD SOLID_INLINE bool operator==(const FFlecsId Other) const
	{
		return GetFlecsId() == Other;
	}

	NO_DISCARD SOLID_INLINE bool operator!=(const FFlecsId Other) const
	{
		return GetFlecsId() != Other;
	}
	
	SOLID_INLINE flecs::entity operator->() const
	{
		return GetEntity();
	}

	NO_DISCARD SOLID_INLINE FString ToString() const
	{
		return FString::Printf(TEXT("Entity: %hs"), GetEntity().str().c_str());
	}

	template <typename TEnum>
	NO_DISCARD SOLID_INLINE TEnum ToConstant() const
	{
		return GetEntity().to_constant<TEnum>();
	}

	NO_DISCARD SOLID_INLINE FString ToJson(const bool bSerializePath = true,
		const bool bSerializeLabel = false, const bool bSerializeBrief = false, const bool bSerializeLink = false,
		const bool bSerializeColor = false, const bool bSerializeIds = true, const bool bSerializeIdLabels = false,
		const bool bSerializeBaseComponents = true, const bool bSerializeComponents = true) const
	{
		return FString(GetEntity().to_json().c_str());
	}

	SOLID_INLINE FString FromJson(const FString& InJson) const
	{
		return GetEntity().from_json(Unreal::Flecs::ToCString(InJson));
	}

	// @TODO: Implement serialization
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	template <typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntity().each(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntity().each<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename TSecond>
	SOLID_INLINE const FFlecsEntityHandle& AddPair() const
	{
		GetEntity().add<TFirst, TSecond>();
		return *this;
	}

	template <typename TFirst>
	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FFlecsId InSecond) const
	{
		GetEntity().add<TFirst>(InSecond);
		return *this;
	}

	template <typename TFirst>
	SOLID_INLINE const FFlecsEntityHandle& AddPair(const UScriptStruct* InSecond) const
	{
		AddPair<TFirst>(ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	template <typename TFirst>
	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FGameplayTag& InSecond) const
	{
		AddPair<TFirst>(GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		AddPair(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		AddPair(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		AddPair(ObtainComponentTypeStruct(InFirst), InSecond);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		AddPair(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		AddPair(GetTagEntity(InFirst), GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		AddPair(GetTagEntity(InFirst), InSecond);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		AddPair(InFirst, ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		AddPair(InFirst, GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FFlecsId InFirst, UEnum* InSecond, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(InSecond);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant(InSecond, InValue);
		
		AddPair(EnumEntity, EnumConstant);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		GetEntity().add(InFirst, InSecond);
		return *this;
	}

	template <typename TSecond>
	SOLID_INLINE const FFlecsEntityHandle& AddPairSecond(const FFlecsId InFirst) const
	{
		GetEntity().add_second<TSecond>(InFirst);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		RemovePair(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		RemovePair(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		RemovePair(ObtainComponentTypeStruct(InFirst), InSecond);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		RemovePair(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		RemovePair(GetTagEntity(InFirst), GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		RemovePair(GetTagEntity(InFirst), InSecond);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		RemovePair(InFirst, ObtainComponentTypeStruct(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		RemovePair(InFirst, GetTagEntity(InSecond));
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		GetEntity().remove(InFirst, InSecond);
		return *this;
	}

	template <typename TFirst, typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair() const
	{
		return GetEntity().has<TFirst, TSecond>();
	}

	template <typename TFirst>
	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InSecond) const
	{
		return GetEntity().has<TFirst>(InSecond);
	}

	template <typename TFirst>
	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InSecond) const
	{
		return HasPair<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst>
	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InSecond) const
	{
		return HasPair<TFirst>(GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		return HasPair(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		return HasPair(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		return HasPair(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		return HasPair(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		return HasPair(GetTagEntity(InFirst), GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		return HasPair(GetTagEntity(InFirst), InSecond);
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		return HasPair(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		return HasPair(InFirst, GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		return GetEntity().has(InFirst, InSecond);
	}

	template <typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const FFlecsId InFirst) const
	{
		return GetEntity().has_second<TSecond>(InFirst);
	}

	template <typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const UScriptStruct* InFirst) const
	{
		return HasPairSecond<TSecond>(ObtainComponentTypeStruct(InFirst));
	}

	template <typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const FGameplayTag& InFirst) const
	{
		return HasPairSecond<TSecond>(GetTagEntity(InFirst));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	NO_DISCARD SOLID_INLINE TActual GetPair() const
	{
		return *GetEntity().get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const TSolidNotNull<UScriptStruct*> InSecond) const
	{
		return GetPair<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const FGameplayTag& InSecond) const
	{
		solid_checkf(InSecond.IsValid(), TEXT("Invalid GameplayTag provided for GetPair"));
		solid_checkf(HasPair<TFirst>(InSecond),
			TEXT("Entity does not have pair with %s"), *InSecond.ToString());
		return GetPair<TFirst>(GetTagEntity(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const FFlecsId InSecond) const
	{
		solid_check(HasPair<TFirst>(InSecond));
		return *GetEntity().get<TFirst>(InSecond);
	}
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type
	UE_REQUIRES(std::is_same_v<TSecond, TActual>)>
	SOLID_INLINE TActual GetPairSecond(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		return *GetEntity().get<TFirst, TSecond>(InFirst, InSecond);
	}

	template <typename TSecond, typename TActual = TSecond>
	SOLID_INLINE TActual GetPairSecond(const FFlecsId InFirst) const
	{
		return *GetEntity().get_second<TSecond>(InFirst);
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	SOLID_INLINE TActual* GetPairPtr() const
	{
		return GetEntity().get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual* GetPairPtr(const FFlecsId InSecond) const
	{
		return GetEntity().get_mut<TFirst>(InSecond);
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual* GetPairPtr(const UScriptStruct* InSecond) const
	{
		return GetPairPtr<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual* GetPairPtr(const FGameplayTag& InSecond) const
	{
		return GetPairPtr<TFirst>(GetTagEntity(InSecond));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type
		UE_REQUIRES(std::is_same_v<TFirst, TActual>)>
	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const TActual& InValue) const
	{
		GetEntity().set<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type
		UE_REQUIRES (std::is_same_v<TSecond, TActual>)>
	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const TActual& InValue) const
	{
		GetEntity().set_second<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const FFlecsId InSecond, const TActual& InFirst) const
	{
		GetEntity().set<TFirst>(InSecond, InFirst);
		return *this;
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const UScriptStruct* InSecond, const TActual& InFirst) const
	{
		SetPairFirst<TFirst>(ObtainComponentTypeStruct(InSecond), InFirst);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const UScriptStruct* InFirst, const void* InValue, const UScriptStruct* InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}

		Set(FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond)),
				InFirst->GetStructureSize(),
				InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const UScriptStruct* InFirst,
		const void* InValue, const FGameplayTag& InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}

		Set(FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond)),
				InFirst->GetStructureSize(),
				InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetPairFirst(const UScriptStruct* InFirst,
		const void* InValue, const FFlecsId InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}

		Set(FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst), InSecond),
				InFirst->GetStructureSize(),
				InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = TSecond>
	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const TActual& InSecond) const
	{
		GetEntity().set_second<TFirst, TSecond>(InSecond);
		return *this;
	}

	template <typename TSecond>
	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const FFlecsId InFirst, const TSecond& InValue) const
	{
		GetEntity().set_second<TSecond>(InFirst, InValue);
		return *this;
	}

	template <typename TSecond>
	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const UScriptStruct* InFirst, const TSecond& InValue) const
	{
		SetPairSecond<TSecond>(ObtainComponentTypeStruct(InFirst), InValue);
		return *this;
	}
	
	template <typename TSecond>
	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const FGameplayTag& InFirst, const TSecond& InValue) const
	{
		SetPairSecond<TSecond>(GetTagEntity(InFirst), InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const FFlecsId InFirst,
		const UScriptStruct* InSecond, const void* InValue) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}

		Set(FFlecsId::MakePair(InFirst, ObtainComponentTypeStruct(InSecond)),
			InSecond->GetStructureSize(),
			InValue);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& SetPairSecond(const FGameplayTag& InFirst, const UScriptStruct* InSecond, const void* InValue) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}

		Set(FFlecsId::MakePair(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond)),
			InSecond->GetStructureSize(),
			InValue);
		return *this;
	}
	
	template <typename TFirst, typename TSecond>
	SOLID_INLINE void ModifiedPair() const
	{
		GetEntity().modified<TFirst, TSecond>();
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		ModifiedPair(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		ModifiedPair(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		ModifiedPair(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		ModifiedPair(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		ModifiedPair(GetTagEntity(InFirst), GetTagEntity(InSecond));}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		ModifiedPair(GetTagEntity(InFirst), InSecond);
	}

	SOLID_INLINE void ModifiedPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		ModifiedPair(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		ModifiedPair(InFirst, GetTagEntity(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		GetEntity().modified(InFirst, InSecond);
	}
	
	template <typename TComponent>
	SOLID_INLINE void Modified() const
	{
		GetEntity().modified<TComponent>();
	}
	
	SOLID_INLINE void Modified(const UScriptStruct* StructType) const
	{
		Modified(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Modified(const FGameplayTag& InTag) const
	{
		Modified(GetTagEntity(InTag));
	}

	SOLID_INLINE void Modified(const FFlecsId InEntity) const
	{
		GetEntity().modified(InEntity);
	}

	NO_DISCARD SOLID_INLINE int32 GetDepth(const FFlecsId InEntity) const
	{
		return GetEntity().depth(InEntity);
	}

	NO_DISCARD SOLID_INLINE int32 GetDepth(const UScriptStruct* StructType) const
	{
		return GetDepth(ObtainComponentTypeStruct(StructType));
	}

	NO_DISCARD SOLID_INLINE int32 GetDepth(const FGameplayTag& InTag) const
	{
		return GetDepth(GetTagEntity(InTag));
	}

	template <typename TEntity>
	NO_DISCARD SOLID_INLINE int32 GetDepth() const
	{
		return GetEntity().depth<TEntity>();
	}

	NO_DISCARD SOLID_INLINE FString GetPath() const
	{
		return FString(GetEntity().path());
	}

	NO_DISCARD SOLID_INLINE FString GetPath(const FString& InSeparator, const FString& InitialSeparator) const
	{
		return FString(GetEntity().path(Unreal::Flecs::ToCString(InSeparator),
								Unreal::Flecs::ToCString(InitialSeparator)));
	}

	NO_DISCARD FFlecsEntityHandle ObtainComponentTypeStruct(const TSolidNotNull<const UScriptStruct*> StructType) const;
	
	NO_DISCARD FFlecsEntityHandle ObtainComponentTypeEnum(const TSolidNotNull<const UEnum*> EnumType) const;

	template <typename TEnumUnderlying = uint64>
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle ObtainEnumConstant(const TSolidNotNull<const UEnum*> EnumType,
	                                                              const TEnumUnderlying InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		return EnumEntity.Lookup(EnumType->GetNameStringByValue(static_cast<int64>(InValue)));
	}

	SOLID_INLINE const FFlecsEntityHandle& AddPrefab(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	SOLID_INLINE const FFlecsEntityHandle& RemovePrefab(const FFlecsId InPrefab) const
	{
		RemovePair(flecs::IsA, InPrefab);
		return *this;
	}
	
	SOLID_INLINE const FFlecsEntityHandle& SetIsA(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FFlecsEntityHandle& SetIsA() const
	{
		GetEntity().is_a<T>();
		return *this;
	}

	NO_DISCARD SOLID_INLINE bool HasPrefab(const FFlecsId InPrefab) const
	{
		return HasPair(flecs::IsA, InPrefab);
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Lookup(const FString& InPath, const bool bSearchPath = false) const
	{
		return GetEntity().lookup(Unreal::Flecs::ToCString(InPath), bSearchPath);
	}

	void AddCollection(UObject* Collection) const;
	
protected:
	flecs::entity Entity;
	
	NO_DISCARD FFlecsEntityHandle GetTagEntity(const FGameplayTag& InTag) const;

	NO_DISCARD SOLID_INLINE flecs::world GetFlecsWorld_Internal() const
	{
		return Entity.world();
	}
	
}; // struct FFlecsEntityHandle

template <>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithNetSerializer = true,
	}; // enum
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
