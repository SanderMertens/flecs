// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Types/SolidNotNull.h"

#include "General/FlecsStringConverters.h"

#include "FlecsEntityHandleTypes.h"
#include "FlecsCommonHandle.h"
#include "FlecsArchetype.h"

#include "FlecsEntityView.generated.h"

enum class EFlecsAccessorType : uint8
{
	// Reference
	Ref = 1 << 0,
	// Pointer
	Ptr = 1 << 1,

	// Mutable
	Mut = 1 << 2,
	// Constant
	Const = 1 << 3,

	ConstRef = (Ref | Const),
	ConstPtr = (Ptr | Const),
	MutRef = (Ref | Mut),
	MutPtr = (Ptr | Mut),
	
}; // enum class EFlecsAccessorType
ENUM_CLASS_FLAGS(EFlecsAccessorType)

namespace Unreal::Flecs::Accessors
{
	constexpr NO_DISCARD bool IsRef(const EFlecsAccessorType InType)
	{
		return EnumHasAnyFlags(InType, EFlecsAccessorType::Ref);
	}

	constexpr NO_DISCARD bool IsPtr(const EFlecsAccessorType InType)
	{
		return EnumHasAnyFlags(InType, EFlecsAccessorType::Ptr);
	}

	constexpr NO_DISCARD bool IsMut(const EFlecsAccessorType InType)
	{
		return EnumHasAnyFlags(InType, EFlecsAccessorType::Mut);
	}

	constexpr NO_DISCARD bool IsConst(const EFlecsAccessorType InType)
	{
		return EnumHasAnyFlags(InType, EFlecsAccessorType::Const);
	}

	constexpr NO_DISCARD bool IsConstRef(const EFlecsAccessorType InType)
	{
		return IsRef(InType) && IsConst(InType);
	}

	constexpr NO_DISCARD bool IsConstPtr(const EFlecsAccessorType InType)
	{
		return IsPtr(InType) && IsConst(InType);
	}

	constexpr NO_DISCARD bool IsMutRef(const EFlecsAccessorType InType)
	{
		return IsRef(InType) && IsMut(InType);
	}

	constexpr NO_DISCARD bool IsMutPtr(const EFlecsAccessorType InType)
	{
		return IsPtr(InType) && IsMut(InType);
	}

	template <typename TQualifier>
	struct TAccessorHelper;

	template <typename TFirst, typename TSecond, typename TQualifier>
	struct TPairAccessorHelper;
	
} // namespace Unreal::Flecs::Accessors

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsEntityView : public FFlecsCommonHandle
{
	GENERATED_BODY()

	using FSelfType = FFlecsEntityView;
	
	static NO_DISCARD SOLID_INLINE FFlecsEntityView GetNullHandle()
	{
		return FFlecsEntityView(flecs::entity::null());
	}

	static NO_DISCARD FFlecsEntityView GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld);

public:
	SOLID_INLINE FFlecsEntityView() = default;
	
	SOLID_INLINE FFlecsEntityView(const flecs::entity& InEntity) : FFlecsCommonHandle(InEntity)
	{
	}
	
	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const flecs::entity& InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world_t* InWorld, const flecs::entity_t InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE explicit FFlecsEntityView(const FFlecsId InEntity) : FFlecsCommonHandle(InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::world& InWorld, const FFlecsId InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity)
		: FFlecsCommonHandle(InWorld, InEntity)
	{
	}

	SOLID_INLINE FFlecsEntityView(const flecs::entity_view& View)
		: FFlecsCommonHandle(View)
	{
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::entity_view& InView) const
	{
		return GetEntityView().mut(InView);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::world& InWorld) const
	{
		return GetEntityView().mut(InWorld);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD THandle ToMut(const flecs::iter& InIter) const
	{
		return GetEntityView().mut(InIter);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool Has() const
	{
		return GetEntityView().has<T>();
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	SOLID_INLINE bool Has(const T& InValue) const
	{
		return GetEntityView().has(FFlecsCommonHandle::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType) const
	{
		return HasPair(FFlecsCommonHandle::GetInputId(*this, EnumType), flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool Has(const UEnum* EnumType, const int64 InValue) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const FFlecsId EnumConstant = ObtainEnumConstant<FFlecsId>(EnumType, InValue);
		
		return HasPair(EnumEntity, EnumConstant);
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE bool Has(const T InValue) const
	{
		return GetEntityView().has<T>(InValue);
	}

	NO_DISCARD SOLID_INLINE bool Has(const FSolidEnumSelector& EnumSelector) const
	{
		return Has(EnumSelector.Class, EnumSelector.Value);
	}

	template <typename TFirst, typename TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair() const
	{
		return GetEntityView().has<TFirst, TSecond>();
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TSecond& InSecond) const
	{
		return GetEntityView().has<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}
	
	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	NO_DISCARD SOLID_INLINE bool HasPair(const TFirst& InFirst, const TSecond& InSecond) const
	{
		return GetEntityView().has(FFlecsEntityView::GetInputId(*this, InFirst),
			FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TSecond, Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	NO_DISCARD SOLID_INLINE bool HasPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().has_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename T, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstRef>
	requires (Unreal::Flecs::Accessors::IsRef(AccessorType) && Unreal::Flecs::Accessors::IsConst(AccessorType))
	NO_DISCARD SOLID_INLINE const T& Get() const
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		return GetEntityView().get<T>();
	}

	template <typename T, EFlecsAccessorType AccessorType>
	requires (Unreal::Flecs::Accessors::IsRef(AccessorType) && Unreal::Flecs::Accessors::IsMut(AccessorType))
	NO_DISCARD SOLID_INLINE T& Get()
	{
		solid_checkf(Has<T>(),
			TEXT("Entity does not have component with type %hs"), nameof(T).data());
		return GetEntityView().get_mut<T>();
	}

	template <typename T, EFlecsAccessorType AccessorType>
	requires (Unreal::Flecs::Accessors::IsPtr(AccessorType) && Unreal::Flecs::Accessors::IsConst(AccessorType))
	NO_DISCARD SOLID_INLINE const T* Get() const
	{
		return GetEntityView().try_get<T>();
	}

	template <typename T, EFlecsAccessorType AccessorType>
	requires (Unreal::Flecs::Accessors::IsPtr(AccessorType) && Unreal::Flecs::Accessors::IsMut(AccessorType))
	NO_DISCARD SOLID_INLINE T* Get()
	{
		return GetEntityView().try_get_mut<T>();
	}

	template <typename T, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstPtr>
	requires (Unreal::Flecs::Accessors::IsConst(AccessorType))
	NO_DISCARD SOLID_INLINE const T* TryGet() const
	{
		return GetEntityView().try_get<T>();
	}

	template <typename T, EFlecsAccessorType AccessorType>
	requires (Unreal::Flecs::Accessors::IsMut(AccessorType))
	NO_DISCARD SOLID_INLINE T* TryGet()
	{
		return GetEntityView().try_get_mut<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE const void* GetPtr(const T& InTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE void* GetMutPtr(const T& InTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <typename TFunction>
	requires (flecs::is_callable<TFunction>::value)
	SOLID_INLINE bool GetLambda(TFunction&& InFunction) const
	{
		return GetEntityView().get(std::forward<TFunction>(InFunction));
	}

	template <typename T>
	requires (std::is_enum<T>::value)
	NO_DISCARD SOLID_INLINE T GetEnumValue() const
	{
		return GetEntity().get_constant<T>();
	}

	NO_DISCARD SOLID_INLINE uint64 GetEnumValue(const UEnum* EnumType) const
	{
		const FFlecsEntityView Target = GetPairTarget<FFlecsEntityView>(EnumType);
		solid_check(Target.IsValid());
		
		return Target.ToConstant(EnumType);
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstRef,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual> && Unreal::Flecs::Accessors::IsConstRef(AccessorType))
	NO_DISCARD SOLID_INLINE const TActual& GetPairFirst() const
	{
		solid_checkf((HasPair<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntityView().get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual> && Unreal::Flecs::Accessors::IsMutRef(AccessorType))
	NO_DISCARD SOLID_INLINE TActual& GetPairFirst() const
	{
		solid_checkf((HasPair<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntityView().get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstRef,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	requires (Unreal::Flecs::Accessors::IsConstRef(AccessorType))
	NO_DISCARD SOLID_INLINE const TFirst& GetPairFirst(const TSecond& InSecond) const
	{
		solid_checkf(HasPair<TFirst>(InSecond),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TFirst).data(),
			*InSecond.ToString());
		
		return GetEntityView().get<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	requires (Unreal::Flecs::Accessors::IsMutRef(AccessorType))
	NO_DISCARD SOLID_INLINE TFirst& GetPairFirst(const TSecond& InSecond) const
	{
		solid_checkf(HasPair<TFirst>(InSecond),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TFirst).data(),
			*InSecond.ToString());
		
		return GetEntityView().get_mut<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual> && Unreal::Flecs::Accessors::IsConstPtr(AccessorType))
	NO_DISCARD SOLID_INLINE const TActual* GetPairFirst() const
	{
		return GetEntityView().try_get<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TFirst, TActual> && Unreal::Flecs::Accessors::IsMutPtr(AccessorType))
	NO_DISCARD SOLID_INLINE TActual* GetPairFirst() const
	{
		return GetEntityView().try_get_mut<TFirst, TSecond>();
	}

	template <typename TFirst, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	requires (Unreal::Flecs::Accessors::IsConstPtr(AccessorType))
	NO_DISCARD SOLID_INLINE const TFirst* GetPairFirst(const TSecond& InSecond) const
	{
		return GetEntityView().try_get<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <typename TFirst, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TSecond>
	requires (Unreal::Flecs::Accessors::IsMutPtr(AccessorType))
	NO_DISCARD SOLID_INLINE TFirst* GetPairFirst(const TSecond& InSecond) const
	{
		return GetEntityView().try_get_mut<TFirst>(FFlecsEntityView::GetInputId(*this, InSecond));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* GetPairPtrFirst(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* GetPairPtrMutFirst(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}
	
	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstRef,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual> && Unreal::Flecs::Accessors::IsConstRef(AccessorType))
	NO_DISCARD SOLID_INLINE const TActual& GetPairSecond() const
	{
		solid_checkf((HasPairSecond<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntityView().get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual> && Unreal::Flecs::Accessors::IsMutRef(AccessorType))
	NO_DISCARD SOLID_INLINE TActual& GetPairSecond() const
	{
		solid_checkf((HasPairSecond<TFirst, TSecond>()),
			TEXT("Entity does not have pair with %hs and %hs"),
			nameof(TFirst).data(),
			nameof(TSecond).data());
		
		return GetEntityView().get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, EFlecsAccessorType AccessorType = EFlecsAccessorType::ConstRef,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	requires (Unreal::Flecs::Accessors::IsConstRef(AccessorType))
	NO_DISCARD SOLID_INLINE const TSecond& GetPairSecond(const TFirst& InFirst) const
	{
		solid_checkf(HasPairSecond<TSecond>(InFirst),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TSecond).data(),
			*InFirst.ToString());
		
		return GetEntityView().get_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename TSecond, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	requires (Unreal::Flecs::Accessors::IsMutRef(AccessorType))
	NO_DISCARD SOLID_INLINE TSecond& GetPairSecond(const TFirst& InFirst) const
	{
		solid_checkf(HasPairSecond<TSecond>(InFirst),
			TEXT("Entity does not have pair with %hs and %s"),
			nameof(TSecond).data(),
			*InFirst.ToString());
		
		return GetEntityView().get_mut_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}
	
	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual> && Unreal::Flecs::Accessors::IsConstPtr(AccessorType))
	NO_DISCARD SOLID_INLINE const TActual* GetPairSecond() const
	{
		return GetEntityView().try_get_second<TFirst, TSecond>();
	}

	template <typename TFirst, typename TSecond, EFlecsAccessorType AccessorType,
		typename TActual = typename flecs::pair<TFirst, TSecond>::type>
	requires (std::is_same_v<TSecond, TActual> && Unreal::Flecs::Accessors::IsMutPtr(AccessorType))
	NO_DISCARD SOLID_INLINE TActual* GetPairSecond() const
	{
		return GetEntityView().try_get_mut_second<TFirst, TSecond>();
	}

	template <typename TSecond, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	requires (Unreal::Flecs::Accessors::IsConstPtr(AccessorType))
	NO_DISCARD SOLID_INLINE const TSecond* GetPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().try_get_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <typename TSecond, EFlecsAccessorType AccessorType,
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TFirst>
	requires (Unreal::Flecs::Accessors::IsMutPtr(AccessorType))
	NO_DISCARD SOLID_INLINE TSecond* GetPairSecond(const TFirst& InFirst) const
	{
		return GetEntityView().try_get_mut_second<TSecond>(FFlecsEntityView::GetInputId(*this, InFirst));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE const void* GetPairPtrSecond(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First, 
		Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept Second>
	NO_DISCARD SOLID_INLINE void* GetPairPtrMutSecond(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().try_get_mut(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <typename TFirst, Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle GetPairTarget(const int32 Index = 0) const
	{
		return GetEntityView().target<TFirst>(Index);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle,
		Unreal::Flecs::TFlecsEntityFunctionDataTypeWithEnumNoValueConcept TFirst>
	NO_DISCARD SOLID_INLINE THandle GetPairTarget(const TFirst& InFirstTypeValue, const int32 Index = 0) const
	{
		return GetEntityView().target(FFlecsEntityView::GetInputId(*this, InFirstTypeValue), Index);
	}

	NO_DISCARD SOLID_INLINE bool IsComponent() const
	{
		return Has<flecs::Component>();
	}

	NO_DISCARD SOLID_INLINE bool IsTag() const
	{
		return !IsComponent() || Get<flecs::Component, EFlecsAccessorType::ConstRef>().size == 0;
	}

	NO_DISCARD SOLID_INLINE bool IsEnum() const
	{
		return Has<flecs::Enum>();
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle GetParent() const
	{
		return GetEntityView().parent();
	}

	NO_DISCARD SOLID_INLINE bool HasParent() const
	{
		return HasPair(flecs::ChildOf, flecs::Wildcard);
	}

	NO_DISCARD SOLID_INLINE bool IsPrefab() const
	{
		return Has(flecs::Prefab);
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool DoesOwn() const
	{
		return GetEntityView().owns<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept T>
	NO_DISCARD SOLID_INLINE bool DoesOwn(const T& InTypeValue) const
	{
		return GetEntityView().owns(FFlecsEntityView::GetInputId(*this, InTypeValue));
	}

	template <typename First, typename Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair() const
	{
		return GetEntityView().owns<First, Second>();
	}

	template <typename First, Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const Second& InSecondTypeValue) const
	{
		return GetEntityView().owns<First>(FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First,
		Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept Second>
	NO_DISCARD SOLID_INLINE bool DoesOwnPair(const First& InFirstTypeValue, const Second& InSecondTypeValue) const
	{
		return GetEntityView().owns(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			FFlecsEntityView::GetInputId(*this, InSecondTypeValue));
	}

	template <typename Second, Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept First>
	NO_DISCARD SOLID_INLINE bool DoesOwnPairSecond(const First& InFirstTypeValue) const
	{
		return GetEntityView().owns_second<Second>(FFlecsEntityView::GetInputId(*this, InFirstTypeValue));
	}

	// @TODO: Optimize this?
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle, typename TEnumUnderlying = uint64>
	NO_DISCARD SOLID_INLINE THandle ObtainEnumConstant(const TSolidNotNull<const UEnum*> EnumType,
																  const TEnumUnderlying InValue) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(EnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		return EnumEntity.Lookup<THandle>(EnumType->GetNameStringByValue(static_cast<int64>(InValue)));
	}
	
	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle Lookup(const FString& InPath, const bool bSearchPath = false) const
	{
		return GetEntityView().lookup(Unreal::Flecs::ToCString(InPath), bSearchPath);
	}

	template <typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntityView().each(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(FunctionType&& InFunction) const
	{
		GetEntityView().each<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void Iterate(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntityView().each(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	template <typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntityView().children(std::forward<FunctionType>(InFunction));
	}

	template <typename TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(FunctionType&& InFunction) const
	{
		GetEntityView().children<TFirst, FunctionType>(std::forward<FunctionType>(InFunction));
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputDataTypeConcept TFirst, typename FunctionType>
	SOLID_INLINE void IterateChildren(const TFirst& InFirstTypeValue, FunctionType&& InFunction) const
	{
		GetEntityView().children(FFlecsEntityView::GetInputId(*this, InFirstTypeValue),
			std::forward<FunctionType>(InFunction));
	}

	template <typename TEnum>
	NO_DISCARD SOLID_INLINE TEnum ToConstant() const
	{
		return GetEntityView().to_constant<TEnum>();
	}
	
	NO_DISCARD SOLID_INLINE uint64 ToConstant(const TSolidNotNull<const UEnum*> InEnumType) const
	{
		const FFlecsEntityView EnumEntity = ObtainComponentTypeEnum<FFlecsEntityView>(InEnumType);
		solid_check(EnumEntity.IsValid());
		solid_check(EnumEntity.IsEnum());

		const uint64 MaxEnumValue = InEnumType->GetMaxEnumValue();
		
		if (MaxEnumValue <= std::numeric_limits<uint8>::max())
		{
			return GetEntityView().to_constant<uint8>(EnumEntity);
		}
		else
		{
			return GetEntityView().to_constant<uint64>(EnumEntity);
		}
	}

	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntityView().enabled();
	}

	template <typename T>
	NO_DISCARD SOLID_INLINE bool IsEnabled() const
	{
		return GetEntityView().enabled<T>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept T>
	NO_DISCARD SOLID_INLINE bool IsEnabled(const T& InValue) const
	{
		return GetEntityView().enabled(FFlecsEntityView::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE FString GetName() const
	{
		return FString(GetEntityView().name().c_str(), static_cast<int32>(GetEntity().name().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasName() const
	{
		return DoesOwnPair<flecs::Identifier>(flecs::Name);
	}

	NO_DISCARD SOLID_INLINE FString GetSymbol() const
	{
		return FString(GetEntityView().symbol().c_str(), static_cast<int32>(GetEntityView().symbol().length()));
	}
	
	NO_DISCARD SOLID_INLINE bool HasSymbol() const
	{
		return DoesOwnPair<flecs::Identifier>(flecs::Symbol);
	}

	template <Unreal::Flecs::TFlecsEntityHandleTypeConcept THandle>
	NO_DISCARD SOLID_INLINE THandle Clone(const bool bCloneValue = true, const FFlecsId DestinationId = 0) const
	{
		return GetEntityView().clone(bCloneValue, DestinationId);
	}

	NO_DISCARD SOLID_INLINE FString ToJson() const
	{
		return FString(GetEntityView().to_json().c_str());
	}

	NO_DISCARD SOLID_INLINE FString ToJson(const ecs_entity_to_json_desc_t& InDesc) const
	{
		return FString(GetEntityView().to_json(&InDesc).c_str());
	}

	template <typename TEntity>
	NO_DISCARD SOLID_INLINE int32 GetDepth() const
	{
		return GetEntityView().depth<TEntity>();
	}

	template <Unreal::Flecs::TFlecsEntityFunctionInputTypeConcept TComponent>
	NO_DISCARD SOLID_INLINE int32 GetDepth(const TComponent& InValue) const
	{
		return GetEntityView().depth(FFlecsEntityView::GetInputId(*this, InValue));
	}

	NO_DISCARD SOLID_INLINE FString GetPath() const
	{
		return FString(GetEntityView().path());
	}

	NO_DISCARD SOLID_INLINE FString GetPath(const FString& InSeparator, const FString& InitialSeparator) const
	{
		return FString(GetEntityView().path(Unreal::Flecs::ToCString(InSeparator),
								Unreal::Flecs::ToCString(InitialSeparator)));
	}

	NO_DISCARD SOLID_INLINE FFlecsArchetype GetType() const
	{
		return FFlecsArchetype(GetEntityView().type());
	}

#if defined(FLECS_DOC)

	NO_DISCARD SOLID_INLINE FString GetDocBrief() const
	{
		return GetEntityView().doc_brief();
	}

	NO_DISCARD SOLID_INLINE FString GetDocColor() const
	{
		return GetEntityView().doc_color();
	}

	NO_DISCARD SOLID_INLINE FString GetDocName() const
	{
		return GetEntityView().doc_name();
	}

	NO_DISCARD SOLID_INLINE FString GetDocLink() const
	{
		return GetEntityView().doc_link();
	}

	NO_DISCARD SOLID_INLINE FString GetDocDetails() const
	{
		return GetEntityView().doc_detail();
	}

#endif // #if defined(FLECS_DOC)
	
}; // struct FFlecsEntityView
