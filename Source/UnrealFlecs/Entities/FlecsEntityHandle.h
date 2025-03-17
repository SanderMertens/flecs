// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "flecs.h"
#include "FlecsArchetype.h"
#include "FlecsId.h"
#include "GameplayTagContainer.h"
#include "SolidMacros/Macros.h"
#include "StructUtils/InstancedStruct.h"
#include "Types/SolidEnumSelector.h"
#include "FlecsEntityHandle.generated.h"

class UFlecsWorld;

/**
 * @struct FFlecsEntityHandle
 *
 * A handle for managing flecs entities in Unreal Engine with added blueprint support.
 * The structure provides several utility functions to interact with flecs entities,
 * including validation, component addition/removal, and direct data access.
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

	NO_DISCARD static FFlecsEntityHandle GetNullHandle(const UFlecsWorld* InWorld);

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

	FFlecsEntityHandle(const UFlecsWorld* InWorld, const FFlecsId InEntity);

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
		return IsValid();
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
	
	NO_DISCARD UFlecsWorld* GetFlecsWorld() const;
	NO_DISCARD UWorld* GetOuterWorld() const;
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

		const FFlecsId EnumConstant = EnumEntity.Lookup(EnumType->GetNameStringByValue(InValue));
		
		return HasPair(EnumEntity, EnumConstant);
	}

	NO_DISCARD SOLID_INLINE bool Has(const FSolidEnumSelector& EnumSelector) const
	{
		return Has(EnumSelector.Class, EnumSelector.Value);
	}

	SOLID_INLINE void Add(const FFlecsId InEntity) const
	{
		GetEntity().add(InEntity);
	}

	SOLID_INLINE void Add(const UScriptStruct* StructType) const
	{
		Add(ObtainComponentTypeStruct(StructType));
	}
	
	SOLID_INLINE void Add(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);

		const FFlecsId ValueEntity = EnumEntity.Lookup(EnumType->GetNameStringByValue(InValue));
		solid_check(ValueEntity.IsValid());
		
		AddPair(EnumEntity, ValueEntity);
	}

	SOLID_INLINE void Add(const FGameplayTag& InTag) const
	{
		Add(GetTagEntity(InTag));
	}

	SOLID_INLINE void Add(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			Add(Tag);
		}
	}
	
	template <typename T>
	SOLID_INLINE void Add() const
	{
		GetEntity().add<T>();
	}

	template <typename T UE_REQUIRES (std::is_enum<T>::value)>
	SOLID_INLINE void Add(const T InValue) const
	{
		GetEntity().add<T>(InValue);
	}
	
	SOLID_INLINE void Remove(const FFlecsId InEntity) const
	{
		GetEntity().remove(InEntity);
	}

	SOLID_INLINE void Remove(const UScriptStruct* StructType) const
	{
		Remove(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Remove(const FGameplayTag& InTag) const
	{
		Remove(GetTagEntity(InTag));
	}

	SOLID_INLINE void Remove(const UEnum* EnumType) const
	{
		RemovePair(ObtainComponentTypeEnum(EnumType), flecs::Wildcard);
	}

	SOLID_INLINE void Remove(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = EnumEntity.Lookup(EnumType->GetNameStringByValue(InValue));
		
		RemovePair(EnumEntity, EnumConstant);
	}

	SOLID_INLINE void Remove(const FGameplayTagContainer& InTags, const bool bMustHaveAll = false) const
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
	}

	template <typename T>
	SOLID_INLINE void Remove() const
	{
		GetEntity().remove<T>();
	}

	template <typename First, typename Second>
	SOLID_INLINE void RemovePair() const
	{
		GetEntity().remove<First, Second>();
	}

	template <typename First>
	SOLID_INLINE void RemovePair(const FFlecsId InSecond) const
	{
		GetEntity().remove<First>(InSecond);
	}

	template <typename T>
	SOLID_INLINE void Set(const T& InValue) const
	{
		GetEntity().set<T>(InValue);
	}

	SOLID_INLINE void Set(const FFlecsId InId, const void* InValue) const
	{
		GetEntity().set_ptr(InId, InValue);
	}
	
	SOLID_INLINE void Set(const UScriptStruct* StructType, const void* InValue) const
	{
		Set(ObtainComponentTypeStruct(StructType), InValue);
	}

	SOLID_INLINE void Set(const FInstancedStruct& InValue) const
	{
		Set(ObtainComponentTypeStruct(InValue.GetScriptStruct()), InValue.GetMemory());
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

	SOLID_INLINE void Enable() const
	{
		GetEntity().enable();
	}
	
	SOLID_INLINE void Disable() const
	{
		GetEntity().disable();
	}

	template <typename T>
	SOLID_INLINE void Enable() const
	{
		GetEntity().enable<T>();
	}

	SOLID_INLINE void Enable(const FFlecsId InEntity) const
	{
		GetEntity().enable(InEntity);
	}
	
	SOLID_INLINE void Enable(const UScriptStruct* StructType) const
	{
		Enable(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Enable(const FGameplayTag& InTag) const
	{
		Enable(GetTagEntity(InTag));
	}

	SOLID_INLINE void Disable(const FFlecsId InEntity) const
	{
		GetEntity().disable(InEntity);
	}
	
	SOLID_INLINE void Disable(const UScriptStruct* StructType) const
	{
		Disable(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Disable(const FGameplayTag& InTag) const
	{
		Disable(GetTagEntity(InTag));
	}

	template <typename T>
	SOLID_INLINE void Disable() const
	{
		GetEntity().disable<T>();
	}

	SOLID_INLINE void Toggle() const
	{
		IsEnabled() ? Disable() : Enable();
	}

	template <typename T>
	SOLID_INLINE void Toggle() const
	{
		IsEnabled<T>() ? Disable<T>() : Enable<T>();
	}

	SOLID_INLINE void Toggle(const FFlecsId InEntity) const
	{
		GetEntity().enable(InEntity, !IsEnabled(InEntity));
	}
	
	SOLID_INLINE void Toggle(const UScriptStruct* StructType) const
	{
		Toggle(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Toggle(const FGameplayTag& InTag) const
	{
		Toggle(GetTagEntity(InTag));
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

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Clone(const bool bCloneValue = true, const FFlecsId DestinationId = 0) const
	{
		return GetEntity().clone(bCloneValue, DestinationId);
	}

	SOLID_INLINE void SetName(const FString& InName) const
	{
		GetEntity().set_name(StringCast<char>(*InName).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetName() const
	{
		return FString(GetEntity().name());
	}
	
	NO_DISCARD SOLID_INLINE bool HasName() const
	{
		return HasPair<flecs::Identifier>(flecs::Name);
	}

	NO_DISCARD SOLID_INLINE FString GetSymbol() const
	{
		return FString(GetEntity().symbol().c_str());
	}
	
	NO_DISCARD SOLID_INLINE bool HasSymbol() const
	{
		return HasPair<flecs::Identifier>(flecs::Symbol);
	}

	SOLID_INLINE void SetDocBrief(const FString& InDocBrief) const
	{
		GetEntity().set_doc_brief(StringCast<char>(*InDocBrief).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocBrief() const
	{
		return FString(GetEntity().doc_brief());
	}

	SOLID_INLINE void SetDocColor(const FString& Link) const
	{
		GetEntity().set_doc_color(StringCast<char>(*Link).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocColor() const
	{
		return FString(GetEntity().doc_color());
	}

	SOLID_INLINE void SetDocName(const FString& InDocName) const
	{
		GetEntity().set_doc_name(StringCast<char>(*InDocName).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocName() const
	{
		return FString(GetEntity().doc_name());
	}

	SOLID_INLINE void SetDocLink(const FString& InDocLink) const
	{
		GetEntity().set_doc_link(StringCast<char>(*InDocLink).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocLink() const
	{
		return FString(GetEntity().doc_link());
	}

	SOLID_INLINE void SetDocDetails(const FString& InDocDetails) const
	{
		GetEntity().set_doc_detail(StringCast<char>(*InDocDetails).Get());
	}
	
	NO_DISCARD SOLID_INLINE FString GetDocDetails() const
	{
		return FString(GetEntity().doc_detail());
	}
	
	NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetParent() const
	{
		return GetEntity().parent();
	}

	NO_DISCARD SOLID_INLINE bool HasParent() const
	{
		return HasPair(flecs::ChildOf, flecs::Wildcard);
	}

	SOLID_INLINE void SetParent(const FFlecsId InParent) const
	{
		GetEntity().child_of(InParent);
	}

	SOLID_INLINE void SetParent(const FFlecsId InParent, const bool bIsA) const
	{
		GetEntity().child_of(InParent);

		if (bIsA)
		{
			GetEntity().is_a(InParent);
		}
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
		return GetEntity().owns(ObtainComponentTypeStruct(First), Second);
	}

	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const FFlecsId First, const UScriptStruct* Second) const
	{
		return GetEntity().owns(First, ObtainComponentTypeStruct(Second));
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

	template <typename FunctionType>
	SOLID_INLINE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe(std::forward<FunctionType>(InFunction));
	}

	template <typename TEvent, typename FunctionType>
	SOLID_INLINE void Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe<TEvent>(std::forward<FunctionType>(InFunction));
	}

	template <typename FunctionType>
	SOLID_INLINE void Observe(const FFlecsId InEntity, FunctionType&& InFunction) const
	{
		GetEntity().observe(InEntity, std::forward<FunctionType>(InFunction));
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

	SOLID_INLINE void FromJson(const FString& InJson) const
	{
		GetEntity().from_json(StringCast<char>(*InJson).Get());
	}

	// @TODO: Implement serialization
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	template <typename FunctionType>
	SOLID_INLINE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each(InFunction);
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(const FunctionType& InFunction) const
	{
		GetEntity().each<TFirst, FunctionType>(InFunction);
	}

	template <typename TFirst, typename TSecond>
	SOLID_INLINE void AddPair() const
	{
		GetEntity().add<TFirst, TSecond>();
	}

	template <typename TFirst>
	SOLID_INLINE void AddPair(const FFlecsId InSecond) const
	{
		GetEntity().add<TFirst>(InSecond);
	}

	template <typename TFirst>
	SOLID_INLINE void AddPair(const UScriptStruct* InSecond) const
	{
		GetEntity().add<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst>
	SOLID_INLINE void AddPair(const FGameplayTag& InSecond) const
	{
		GetEntity().add<TFirst>(GetTagEntity(InSecond));
	}

	SOLID_INLINE void AddPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().add(ObtainComponentTypeStruct(InFirst),
			ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void AddPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().add(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void AddPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		GetEntity().add(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	SOLID_INLINE void AddPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().add(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void AddPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().add(GetTagEntity(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void AddPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		GetEntity().add(GetTagEntity(InFirst), InSecond);
	}

	SOLID_INLINE void AddPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().add(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void AddPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().add(InFirst, GetTagEntity(InSecond));
	}

	SOLID_INLINE void AddPair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		GetEntity().add(InFirst, InSecond);
	}

	template <typename TSecond>
	SOLID_INLINE void AddPairSecond(const FFlecsId InFirst) const
	{
		GetEntity().add_second<TSecond>(InFirst);
	}

	SOLID_INLINE void RemovePair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().remove(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void RemovePair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().remove(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void RemovePair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		GetEntity().remove(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	SOLID_INLINE void RemovePair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().remove(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void RemovePair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().remove(GetTagEntity(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void RemovePair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		GetEntity().remove(GetTagEntity(InFirst), InSecond);
	}

	SOLID_INLINE void RemovePair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().remove(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void RemovePair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().remove(InFirst, GetTagEntity(InSecond));
	}

	SOLID_INLINE void RemovePair(const FFlecsId InFirst, const FFlecsId InSecond) const
	{
		GetEntity().remove(InFirst, InSecond);
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
		return GetEntity().has<TFirst>(ObtainComponentTypeStruct(InSecond).GetEntity());
	}

	template <typename TFirst>
	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InSecond) const
	{
		return GetEntity().has<TFirst>(GetTagEntity(InSecond).GetEntity());
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		return GetEntity().has(ObtainComponentTypeStruct(InFirst),
			ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		return GetEntity().has(ObtainComponentTypeStruct(InFirst),
			GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		return GetEntity().has(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		return GetEntity().has(GetTagEntity(InFirst),
			ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		return GetEntity().has(GetTagEntity(InFirst), GetTagEntity(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		return GetEntity().has(GetTagEntity(InFirst), InSecond);
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		return GetEntity().has(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	NO_DISCARD SOLID_INLINE bool HasPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		return GetEntity().has(InFirst, GetTagEntity(InSecond));
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
		return GetEntity().has_second<TSecond>(ObtainComponentTypeStruct(InFirst));
	}

	template <typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const FGameplayTag& InFirst) const
	{
		return GetEntity().has_second<TSecond>(GetTagEntity(InFirst));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	NO_DISCARD SOLID_INLINE TActual GetPair() const
	{
		return GetEntity().get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const UScriptStruct* InSecond) const
	{
		return GetEntity().get<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const FGameplayTag& InSecond) const
	{
		return GetEntity().get<TFirst>(GetTagEntity(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual GetPair(const FFlecsId InSecond) const
	{
		return GetEntity().get<TFirst>(InSecond);
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
		return GetEntity().get_mut<TFirst>(ObtainComponentTypeStruct(InSecond));
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE TActual* GetPairPtr(const FGameplayTag& InSecond) const
	{
		return GetEntity().get_mut<TFirst>(GetTagEntity(InSecond));
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	SOLID_INLINE void SetPair(const TActual& InValue) const
	{
		GetEntity().set<TFirst, TSecond>(InValue);
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE void SetPair(const FFlecsId InSecond, const TActual& InFirst) const
	{
		GetEntity().set<TFirst>(InSecond, InFirst);
	}

	template <typename TFirst, typename TActual = TFirst>
	SOLID_INLINE void SetPair(const UScriptStruct* InSecond, const TActual& InFirst) const
	{
		GetEntity().set<TFirst>(ObtainComponentTypeStruct(InSecond), InFirst);
	}

	SOLID_INLINE void SetPair(const UScriptStruct* InFirst, const void* InValue, const UScriptStruct* InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}
		
		GetEntity().set_ptr(
			FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst).GetEntity(), ObtainComponentTypeStruct(InSecond).GetEntity()),
			InValue);
	}

	SOLID_INLINE void SetPair(const UScriptStruct* InFirst, const void* InValue, const FGameplayTag& InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}
		
		GetEntity().set_ptr(
			FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst).GetEntity(), GetTagEntity(InSecond).GetEntity()),
			InValue);
	}

	SOLID_INLINE void SetPair(const UScriptStruct* InFirst, const void* InValue, const FFlecsId InSecond) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}
		
		GetEntity().set_ptr(
			FFlecsId::MakePair(ObtainComponentTypeStruct(InFirst).GetEntity(), InSecond), InValue);
	}

	template <typename TFirst, typename TSecond, typename TActual = TSecond>
	SOLID_INLINE void SetPairSecond(const TActual& InSecond) const
	{
		GetEntity().set_second<TFirst, TSecond>(InSecond);
	}

	SOLID_INLINE void SetPairSecond(const FFlecsId InFirst, const UScriptStruct* InSecond, const void* InValue) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}
		
		GetEntity().set_ptr(FFlecsId::MakePair(InFirst, ObtainComponentTypeStruct(InSecond).GetEntity()),
			InSecond->GetStructureSize(), InValue);
	}

	SOLID_INLINE void SetPairSecond(const FGameplayTag& InFirst, const UScriptStruct* InSecond, const void* InValue) const
	{
		if (!HasPair(InFirst, InSecond))
		{
			AddPair(InFirst, InSecond);
		}
		
		GetEntity().set_ptr(
			FFlecsId::MakePair(GetTagEntity(InFirst).GetEntity(), ObtainComponentTypeStruct(InSecond).GetEntity()), InValue);
	}
	
	template <typename TFirst, typename TSecond>
	SOLID_INLINE void ModifiedPair() const
	{
		GetEntity().modified<TFirst, TSecond>();
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().modified(ObtainComponentTypeStruct(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().modified(ObtainComponentTypeStruct(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const UScriptStruct* InFirst, const FFlecsId InSecond) const
	{
		GetEntity().modified(ObtainComponentTypeStruct(InFirst), InSecond);
	}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().modified(GetTagEntity(InFirst), ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().modified(GetTagEntity(InFirst), GetTagEntity(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FGameplayTag& InFirst, const FFlecsId InSecond) const
	{
		GetEntity().modified(GetTagEntity(InFirst), InSecond);
	}

	SOLID_INLINE void ModifiedPair(const FFlecsId InFirst, const UScriptStruct* InSecond) const
	{
		GetEntity().modified(InFirst, ObtainComponentTypeStruct(InSecond));
	}

	SOLID_INLINE void ModifiedPair(const FFlecsId InFirst, const FGameplayTag& InSecond) const
	{
		GetEntity().modified(InFirst, GetTagEntity(InSecond));
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
		GetEntity().modified(ObtainComponentTypeStruct(StructType));
	}

	SOLID_INLINE void Modified(const FGameplayTag& InTag) const
	{
		GetEntity().modified(GetTagEntity(InTag));
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
		return GetEntity().depth(ObtainComponentTypeStruct(StructType));
	}

	NO_DISCARD SOLID_INLINE int32 GetDepth(const FGameplayTag& InTag) const
	{
		return GetEntity().depth(GetTagEntity(InTag));
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
		return FString(GetEntity().path(StringCast<char>(*InSeparator).Get(),
			StringCast<char>(*InitialSeparator).Get()));
	}

	NO_DISCARD FFlecsEntityHandle ObtainComponentTypeStruct(const UScriptStruct* StructType) const;
	NO_DISCARD FFlecsEntityHandle ObtainComponentTypeEnum(const UEnum* EnumType) const;

	SOLID_INLINE void AddPrefab(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
	}

	SOLID_INLINE void RemovePrefab(const FFlecsId InPrefab) const
	{
		RemovePair(flecs::IsA, InPrefab);
	}

	NO_DISCARD SOLID_INLINE bool HasPrefab(const FFlecsId InPrefab) const
	{
		return HasPair(flecs::IsA, InPrefab);
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityHandle Lookup(const FString& InPath, const bool bSearchPath = false) const
	{
		return GetEntity().lookup(StringCast<char>(*InPath).Get(), bSearchPath);
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
