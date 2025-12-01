// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
#pragma once

#include <functional>

#include "flecs.h"

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

#include "SolidMacros/Macros.h"
#include "Concepts/SolidConcepts.h"
#include "Types/SolidEnumSelector.h"
#include "Types/SolidNotNull.h"

#include "FlecsEntityHandleTypes.h"
#include "FlecsEntityView.h"
#include "FlecsId.h"
#include "Logging/StructuredLog.h"

#include "FlecsEntityHandle.generated.h"

struct FFlecsEntityHandle;

struct FFlecsCollectionReference;
struct FFlecsCollectionInstancedReference;

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
struct alignas(8) UNREALFLECS_API FFlecsEntityHandle : public FFlecsEntityView
{
	GENERATED_BODY()

	using FSelfType = FFlecsEntityHandle;

public:

	static NO_DISCARD SOLID_INLINE FFlecsEntityHandle GetNullHandle()
	{
		return FFlecsEntityHandle(flecs::entity::null());
	}

	static NO_DISCARD SOLID_INLINE FFlecsEntityHandle Invalid()
	{
		return FFlecsEntityHandle(flecs::entity::null());
	}

	 static NO_DISCARD FFlecsEntityHandle GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	FFlecsEntityHandle() = default;
	
	SOLID_INLINE FFlecsEntityHandle(const flecs::entity& InEntity) : FFlecsEntityView(InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsEntityHandle(const FFlecsId InEntity) : FFlecsEntityView(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityHandle(const flecs::world& InWorld, const FFlecsId InEntity)
		: FFlecsEntityView(InWorld, InEntity)
	{
	}

	FFlecsEntityHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity);

	FFlecsEntityHandle(const flecs::world_t* InWorld, const FFlecsId InEntity);
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Add(const T& InValue) const
	{
		GetEntity().add(FFlecsCommonHandle::GetInputId(*this, InValue));
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Add(const UEnum* EnumType, const uint64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);

		const FFlecsId ValueEntity = ObtainEnumConstant<FFlecsId>(EnumType, InValue);
		solid_check(ValueEntity.IsValid());
		
		AddPair(EnumEntity, ValueEntity);
		return *this;
	}

	SOLID_INLINE const FSelfType& Add(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			Add(Tag);
		}

		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Add() const
	{
		GetEntity().add<T>();
		return *this;
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	SOLID_INLINE const FSelfType& Add(const T InValue) const
	{
		GetEntity().add<T>(InValue);
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Remove() const
	{
		GetEntity().remove<T>();
		return *this;
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	SOLID_INLINE const FSelfType& Remove() const
	{
		RemovePair<T>(flecs::Wildcard);
		return *this;
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Remove(const T& InValue) const
	{
		GetEntity().remove(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const UEnum* EnumType) const
	{
		RemovePair(FFlecsEntityHandle::GetInputId(*this, EnumType), flecs::Wildcard);
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant<FFlecsId>(EnumType, InValue);
		
		RemovePair(EnumEntity, EnumConstant);
		return *this;
	}

	SOLID_INLINE const FSelfType& Remove(const FSolidEnumSelector& EnumSelector) const
	{
		return Remove(EnumSelector.Class, EnumSelector.Value);
	}

	SOLID_INLINE const FSelfType& Remove(const FGameplayTagContainer& InTags) const
	{
		for (const FGameplayTag& Tag : InTags)
		{
			if (!Has(Tag))
			{
				continue;
			}
			
			Remove(Tag);
		}

		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Set(const T& InValue) const
	{
		GetEntity().set<T>(InValue);
		return *this;
	}

	template <typename T>
	requires (std::is_move_constructible_v<T> && !std::is_lvalue_reference_v<T>)
	SOLID_INLINE const FSelfType& Set(T&& InValue) const  // NOLINT(cppcoreguidelines-missing-std-forward)
	{
		GetEntity().set(FLECS_FWD(InValue));
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Set(const FFlecsId InId, const uint32 InSize, const void* InValue) const
	{
		GetEntity().set_ptr(InId, InSize, InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	SOLID_INLINE const FSelfType& Set(const T& InTypeValue, const void* InData) const
	{
		const FFlecsId InId = FFlecsEntityHandle::GetInputId(*this, InTypeValue);

		if constexpr (std::is_convertible_v<T, const UScriptStruct*>)
		{
			Set(InId, InTypeValue->GetStructureSize(), InData);
		}
		else
		{
			GetEntity().set_ptr(InId, InData);
		}
		
		return *this;
	}

	SOLID_INLINE const FSelfType& Set(const FInstancedStruct& InValue) const
	{
		Set(FFlecsEntityHandle::GetInputId(*this, InValue.GetScriptStruct()),
			InValue.GetScriptStruct()->GetStructureSize(),
			InValue.GetMemory());
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Assign(const T& InValue) const
	{
		GetEntity().assign<T>(InValue);
		return *this;
	}

	template <typename T>
	requires (std::is_move_constructible_v<T> && !std::is_lvalue_reference_v<T>)
	SOLID_INLINE const FSelfType& Assign(T&& InValue) const
	{
		GetEntity().assign<T>(FLECS_FWD(InValue));
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Assign(const FFlecsId InEntity, const uint32 InSize, const void* InValue) const
	{
		GetEntity().set_ptr(InEntity, InSize, InValue);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	SOLID_INLINE const FSelfType& Assign(const T& InTypeValue, const void* InValue) const
	{
		const FFlecsId InId = FFlecsEntityHandle::GetInputId(*this, InTypeValue);
		
		if constexpr (std::is_convertible_v<T, const UScriptStruct*>)
		{
			Assign(InId, InTypeValue->GetStructureSize(), InValue);
		}
		else
		{
			GetEntity().set_ptr(InId, InValue);
		}
		
		return *this;
	}

	SOLID_INLINE const FSelfType& Assign(const FInstancedStruct& InValue) const
	{
		Assign(InValue.GetScriptStruct(), InValue.GetMemory());
		return *this;
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE const FSelfType& Insert(const TFunction& InFunction) const
	{
		GetEntity().insert(InFunction);
		return *this;
	}

	template <typename T, typename ... Args, typename TActual = flecs::actual_type_t<T>>
	SOLID_INLINE const FSelfType& Emplace(Args&& ... InArgs) const
	{
		GetEntity().emplace<T>(std::forward<Args>(InArgs)...);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename ... Args, typename TActual = flecs::pair<TFirst, TSecond>>
	requires (std::is_same<TFirst, TActual>::value)
	SOLID_INLINE const FSelfType& EmplaceFirst(Args&& ... InArgs) const
	{
		GetEntity().emplace<TFirst, TSecond>(std::forward<Args>(InArgs)...);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond, typename ... Args>
	SOLID_INLINE const FSelfType& EmplaceFirst(const TSecond& InSecondType, Args&& ... InArgs) const
	{
		GetEntity().emplace_first<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondType), std::forward<Args>(InArgs)...);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename ... Args, typename TActual = flecs::pair<TFirst, TSecond>>
	requires (std::is_same<TSecond, TActual>::value)
	SOLID_INLINE const FSelfType& EmplaceSecond(Args&& ... InArgs) const
	{
		GetEntity().emplace<TFirst, TSecond>(std::forward<Args>(InArgs)...);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst, typename ... Args>
	SOLID_INLINE const FSelfType& EmplaceSecond(const TFirst& InFirstType, Args&& ... InArgs) const
	{
		GetEntity().emplace_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstType), std::forward<Args>(InArgs)...);
		return *this;
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE flecs::ref<T> GetFlecsRef() const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		
		return GetEntity().get_ref<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE flecs::untyped_ref GetFlecsRef(const T& InTypeValue) const
	{
		return GetEntity().get_ref(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	SOLID_INLINE void Clear() const
	{
		GetEntity().clear();
	}

	SOLID_INLINE const FSelfType& Enable() const
	{
		GetEntity().enable();
		return *this;
	}
	
	SOLID_INLINE const FSelfType& Disable() const
	{
		GetEntity().disable();
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& Enable() const
	{
		GetEntity().enable<T>();
		return *this;
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Enable(const T& InValue) const
	{
		GetEntity().enable(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}
	
	template <typename T>
	SOLID_INLINE const FSelfType& Disable() const
	{
		GetEntity().disable<T>();
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& Disable(const T& InValue) const
	{
		GetEntity().disable(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	SOLID_INLINE bool Toggle() const
	{
		IsEnabled() ? Disable() : Enable();
		return IsEnabled();
	}

	template <typename T>
	SOLID_INLINE bool Toggle() const
	{
		IsEnabled<T>() ? Disable<T>() : Enable<T>();
		return IsEnabled<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE bool Toggle(const T& InValue) const
	{
		GetEntity().enable(FFlecsEntityHandle::GetInputId(*this, InValue), !IsEnabled(InValue));
		return IsEnabled(InValue);
	}

	/**
	 * @brief Delete an entity.
	 * Entities have to be deleted explicitly, and are not deleted when the
	 * entity object goes out of scope.
	 *
	 * @see ecs_delete()
	 */
	SOLID_INLINE void Destroy() const
	{
		GetEntity().destruct();
	}

	SOLID_INLINE const FSelfType& SetName(const FString& InName) const
	{
		GetEntity().set_name(StringCast<char>(*InName).Get());
		return *this;
	}

	SOLID_INLINE const FSelfType& SetName(const FAnsiStringView InName) const
	{
		GetEntity().set_name(InName.GetData());
		return *this;
	}

	SOLID_INLINE const FSelfType& ClearName() const
	{
		GetEntity().set_name(nullptr);
		return *this;
	}


#if defined(FLECS_DOC)
	
	SOLID_INLINE const FSelfType& SetDocBrief(const FString& InDocBrief) const
	{
		GetEntity().set_doc_brief(StringCast<char>(*InDocBrief).Get());
		return *this;
	}

	// @TODO: make a variation for passing in an unreal color type?
	SOLID_INLINE const FSelfType& SetDocColor(const FString& Link) const
	{
		GetEntity().set_doc_color(StringCast<char>(*Link).Get());
		return *this;
	}

	SOLID_INLINE const FSelfType& SetDocName(const FString& InDocName) const
	{
		GetEntity().set_doc_name(StringCast<char>(*InDocName).Get());
		return *this;
	}

	SOLID_INLINE const FSelfType& SetDocLink(const FString& InDocLink) const
	{
		GetEntity().set_doc_link(StringCast<char>(*InDocLink).Get());
		return *this;
	}

	SOLID_INLINE const FSelfType& SetDocDetails(const FString& InDocDetails) const
	{
		GetEntity().set_doc_detail(StringCast<char>(*InDocDetails).Get());
		return *this;
	}

#endif // #if defined(FLECS_DOC)

	SOLID_INLINE const FSelfType& SetParent(const FFlecsId InParent) const
	{
		GetEntity().child_of(InParent);
		return *this;
	}

	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent() const
	{
		solid_checkf(IsComponent(), TEXT("Entity is not a component"));
		return flecs::untyped_component(GetNativeFlecsWorld(), GetEntity());
	}

	// Does not check if the entity is a component
	NO_DISCARD SOLID_INLINE flecs::untyped_component GetUntypedComponent_Unsafe() const
	{
		return flecs::untyped_component(GetNativeFlecsWorld(), GetEntity());
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

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE void Emit(const T& InValue) const
	{
		GetEntity().emit(FFlecsEntityHandle::GetInputId(*this, InValue));
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

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE void Enqueue(const T& InValue) const
	{
		GetEntity().enqueue(FFlecsEntityHandle::GetInputId(*this, InValue));
	}

	template <typename TEvent, typename FunctionType>
	SOLID_INLINE const FSelfType& Observe(FunctionType&& InFunction) const
	{
		GetEntity().observe<TEvent>(std::forward<FunctionType>(InFunction));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T, typename FunctionType>
	SOLID_INLINE const FSelfType& Observe(const T& InValue, FunctionType&& InFunction) const
	{
		GetEntity().observe(FFlecsEntityHandle::GetInputId(*this, InValue), std::forward<FunctionType>(InFunction));
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

	SOLID_INLINE void ResetHandle()
	{
		Entity = flecs::entity::null();
	}

	SOLID_INLINE FFlecsEntityHandle& operator=(TYPE_OF_NULLPTR)
	{
		ResetHandle();
		return *this;
	}

	SOLID_INLINE FString FromJson(const FString& InJson) const
	{
		return GetEntity().from_json(StringCast<char>(*InJson).Get());
	}
	
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	template <typename TFirst, typename TSecond>
	SOLID_INLINE const FSelfType& AddPair() const
	{
		GetEntity().add<TFirst, TSecond>();
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AddPair(const TSecond& InSecond) const
	{
		GetEntity().add<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AddPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		GetEntity().add(FFlecsEntityHandle::GetInputId(*this, InFirst),
			FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	SOLID_INLINE const FSelfType& AddPair(const FFlecsId InFirst, UEnum* InSecond, const int64 InValue) const
	{
		const FFlecsEntityHandle EnumEntity = ObtainComponentTypeEnum<FFlecsEntityHandle>(InSecond);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant<FFlecsId>(InSecond, InValue);
		
		AddPair(InFirst, EnumConstant);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& AddPairSecond(const TFirst& InFirst) const
	{
		GetEntity().add_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
		return *this;
	}

	template <typename First, typename Second>
	SOLID_INLINE const FSelfType& RemovePair() const
	{
		GetEntity().remove<First, Second>();
		return *this;
	}

	template <typename First, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& RemovePair(const Second& InSecond) const
	{
		GetEntity().remove<First>(FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& RemovePair(const First& InFirst, const Second& InSecond) const
	{
		GetEntity().remove(FFlecsEntityHandle::GetInputId(*this, InFirst),
			FFlecsEntityHandle::GetInputId(*this, InSecond));
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& RemovePairSecond(const TFirst& InFirst) const
	{
		GetEntity().remove_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirst));
		return *this;
	}

	// @TODO: add r-value set apis for pairs
	
	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	SOLID_INLINE const FSelfType& SetPair(const TActual& InValue) const
	{
		GetEntity().set<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& SetPair(const TSecond& InSecondType, const TFirst& InValue) const
	{
		GetEntity().set<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondType), InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& SetPair(const TSecond& InSecondType, const void* InValue) const
	{
		// @TODO: check for Type being registered
		
		Set(FFlecsId::MakePair(flecs::_::type<TFirst>::id(GetNativeFlecsWorld()), FFlecsEntityHandle::GetInputId(*this, InSecondType)),
				InValue);
			
		return *this;
	}

	// @TODO: handle PairIsTag
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& SetPair(const First& InFirstTypeValue, const void* InValue, const Second& InSecondTypeValue) const
	{
		Set(FFlecsId::MakePair(
			FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InValue);
		
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept First, typename TActual = TSecond>
	SOLID_INLINE const FSelfType& SetPairSecond(const First& InFirstType, const TActual& InValue) const
	{
		GetEntity().set_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstType), InValue);
		return *this;
	}

	template <typename TFirst, typename TSecond, typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	SOLID_INLINE const FSelfType& AssignPair(const TActual& InValue) const
	{
		GetEntity().assign<TFirst, TSecond>(InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AssignPair(const TSecond& InSecondType, const TFirst& InValue) const
	{
		GetEntity().assign<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondType), InValue);
		return *this;
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE const FSelfType& AssignPair(const TSecond& InSecondType, const void* InValue) const
	{
		solid_checkf(HasPair<TFirst>(InSecondType), 
			TEXT("Entity does not have pair"));

		Assign(FFlecsId::MakePair(flecs::_::type<TFirst>::id(GetNativeFlecsWorld()),
			FFlecsEntityHandle::GetInputId(*this, InSecondType)),
				InValue);
		
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	SOLID_INLINE const FSelfType& AssignPair(const First& InFirstTypeValue, const void* InValue, const Second& InSecondTypeValue) const
	{
		Assign(FFlecsId::MakePair(
			FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue)),
				InValue);
		return *this;
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE const FSelfType& AssignPairSecond(const TFirst& InFirstType, const TSecond& InValue) const
	{
		GetEntity().assign_second<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstType), InValue);
		return *this;
	}
	
	template <typename TFirst, typename TSecond>
	SOLID_INLINE void ModifiedPair() const
	{
		GetEntity().modified<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE void ModifiedPair(const TSecond& InSecondTypeValue) const
	{
		GetEntity().modified<TFirst>(FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}
	
	template <typename TComponent>
	SOLID_INLINE void Modified() const
	{
		GetEntity().modified<TComponent>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TComponent>
	SOLID_INLINE void Modified(const TComponent& InValueValue) const
	{
		GetEntity().modified(FFlecsEntityHandle::GetInputId(*this, InValueValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	SOLID_INLINE void ModifiedPair(const TFirst& InFirstTypeValue, const TSecond& InSecondTypeValue) const
	{
		GetEntity().modified(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityHandle::GetInputId(*this, InSecondTypeValue));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	SOLID_INLINE void ModifiedPairSecond(const TFirst& InFirstTypeValue) const
	{
		GetEntity().modified<TSecond>(FFlecsEntityHandle::GetInputId(*this, InFirstTypeValue));
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE T& Obtain() const
	{
		return GetEntity().obtain<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE void* Obtain(const T& InTypeValue) const
	{
		return GetEntity().obtain(FFlecsEntityHandle::GetInputId(*this, InTypeValue));
	}

	SOLID_INLINE const FSelfType& AddPrefab(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& AddPrefab() const
	{
		GetEntity().is_a<T>();
		return *this;
	}

	SOLID_INLINE const FSelfType& RemovePrefab(const FFlecsId InPrefab) const
	{
		RemovePair(flecs::IsA, InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& RemovePrefab() const
	{
		RemovePairSecond<T>(flecs::IsA);
		return *this;
	}
	
	SOLID_INLINE const FSelfType& SetIsA(const FFlecsId InPrefab) const
	{
		GetEntity().is_a(InPrefab);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& SetIsA() const
	{
		GetEntity().is_a<T>();
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& SetIsA(const T& InValue) const
	{
		GetEntity().is_a(FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	SOLID_INLINE const FSelfType& MarkSlot() const
	{
		GetEntity().slot();
		return *this;
	}

	SOLID_INLINE const FSelfType& MarkSlot(const FFlecsId InId) const
	{
		GetEntity().slot_of(InId);
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& MarkSlot() const
	{
		GetEntity().slot_of<T>();
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& MarkSlot(const T& InIdType) const
	{
		GetEntity().slot_of(FFlecsEntityHandle::GetInputId(*this, InIdType));
		return *this;
	}

	template <typename T>
	SOLID_INLINE const FSelfType& AddWith() const
	{
		AddPairSecond<T>(flecs::With);
		return *this;
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE const FSelfType& AddWith(const T& InValue) const
	{
		AddPair(flecs::With, FFlecsEntityHandle::GetInputId(*this, InValue));
		return *this;
	}

	template <typename TFunction>
	SOLID_INLINE const FSelfType& Scope(const TFunction& InFunction) const
	{
		GetEntity().scope(InFunction);
		return *this;
	}

	SOLID_INLINE const FSelfType& SetChildOrder(FFlecsId* InOrderArray, const int32 InOrderCount) const
	{
		solid_cassumef(InOrderArray != nullptr || InOrderCount == 0,
			TEXT("InOrder cannot be null if InOrderCount is greater than zero"));
		solid_cassumef(InOrderCount >= 0, TEXT("InOrderCount cannot be negative"));
		
		GetEntity().set_child_order(reinterpret_cast<flecs::id_t*>(InOrderArray), InOrderCount);
		return *this;
	}

	SOLID_INLINE const FSelfType& SetChildOrder(const TArrayView<FFlecsId> InOrderArray) const
	{
		return SetChildOrder(InOrderArray.GetData(), InOrderArray.Num());
	}

	NO_DISCARD SOLID_INLINE FFlecsEntityView ToView() const
	{
		return FFlecsEntityView(GetEntity().view());
	}

	const FSelfType& AddCollection(const FFlecsId InCollection, const FInstancedStruct& InParams = FInstancedStruct()) const;

	template <Solid::TScriptStructConcept TCollectionParams>
	SOLID_INLINE const FSelfType& AddCollection(const FFlecsId InCollection, const TCollectionParams& InParams) const
	{
		return AddCollection(InCollection, FInstancedStruct::Make<TCollectionParams>(InParams));
	}

	SOLID_INLINE const FSelfType& AddCollection(UClass* InCollection, const FInstancedStruct& InParams = FInstancedStruct()) const
	{
		return AddCollection(ObtainTypeClass(InCollection), InParams);
	}

	template <Solid::TScriptStructConcept TCollectionParams>
	SOLID_INLINE const FSelfType& AddCollection(UClass* InCollection, const TCollectionParams& InParams) const
	{
		return AddCollection(InCollection, FInstancedStruct::Make<TCollectionParams>(InParams));
	}

	template <Solid::TStaticClassConcept T>
	SOLID_INLINE const FSelfType& AddCollection(const FInstancedStruct& InParams = FInstancedStruct()) const
	{
		return AddCollection(T::StaticClass(), InParams);
	}

	template <Solid::TStaticClassConcept T, Solid::TScriptStructConcept TCollectionParams>
	SOLID_INLINE const FSelfType& AddCollection(const TCollectionParams& InParams) const
	{
		return AddCollection(T::StaticClass(), FInstancedStruct::Make<TCollectionParams>(InParams));
	}

	const FSelfType& AddCollection(const FFlecsCollectionReference& InCollectionRef, const FInstancedStruct& InParams = FInstancedStruct()) const;
	const FSelfType& AddCollection(const FFlecsCollectionInstancedReference& InCollectionRef) const;

	// Note this doesnt remove overridden components
	const FSelfType& RemoveCollection(const FFlecsId InCollection) const;

	// Note this doesnt remove overridden components
	SOLID_INLINE const FSelfType& RemoveCollection(UClass* InCollection) const
	{
		return RemoveCollection(ObtainTypeClass(InCollection));
	}

	// Note this doesnt remove overridden components
	template <Solid::TStaticClassConcept T>
	SOLID_INLINE const FSelfType& RemoveCollection() const
	{
		return RemoveCollection(T::StaticClass());
	}
	
protected:
	
}; // struct FFlecsEntityHandle

template <>
struct TStructOpsTypeTraits<FFlecsEntityHandle> : public TStructOpsTypeTraitsBase2<FFlecsEntityHandle>
{
	enum
	{
		WithNetSerializer = true,
		WithIdenticalViaEquality = true,
	}; // enum
	
}; // struct TStructOpsTypeTraits<FFlecsEntityHandle>
