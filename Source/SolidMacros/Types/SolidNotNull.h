// Elie Wiese-Namir © 2025. All Rights Reserved.

// Renamed and optimized version of UE’s TNotNull, using Solid’s check macros and [[assume]].

#pragma once

#include <type_traits>

#include "CoreMinimal.h"

#include "SolidMacros/Macros.h"

#ifndef ENABLE_SOLID_NOT_NULL

template <typename T>
using TSolidNotNull = TNotNull<T>;

#else // ENABLE_SOLID_NOT_NULL

template <typename T>
struct TSolidNotNull;

namespace Solid::Private
{
#if DO_CHECK
	
	FORCEINLINE void ReportNotNullPtr()
	{
		UE_LOG(LogCore, Fatal, TEXT("Null assigned to TNotNull"));
		for (;;);
	}

	FORCEINLINE void CheckLoadingNotNullPtr(FArchive& Ar)
	{
		if (Ar.IsLoading())
		{
			UE_LOG(LogCore, Fatal,
				TEXT("Null assigned to TNotNull while reading from archive '%s'"), *Ar.GetArchiveName());
		}
	}

	template <typename... ArgTypes>
		struct TIsTSolidNotNullParam
	{
		static constexpr bool Value = false;
	}; // TIsTNotNullParam

	template <typename T>
	struct TIsTSolidNotNullParam<TSolidNotNull<T>>
	{
		static constexpr bool Value = true;
	}; // TIsTNotNullParam<TNotNull<T>>

	template <typename... ArgTypes>
	inline constexpr bool TIsTNotNullParam_V = TIsTSolidNotNullParam<ArgTypes...>::Value;

	template <typename... ArgTypes>
	concept TIsTSolidNotNullParam_C = TIsTNotNullParam_V<ArgTypes...>;

	template <typename T>
	struct TSolidNotNullBase
	{
		using Sub = TSolidNotNull<T>;

		// Allow implicit conversion to the wrapped value, for compatibility with non-TNotNull APIs
		operator T() const&
		{
			solid_checkf(((Sub*)this)->Val, TEXT("TSolidNotNullBase dereferenced null pointer"));
			return ((Sub*)this)->Val;
		}
		operator T() &&
		{
			solid_checkf(((Sub*)this)->Val, TEXT("TSolidNotNullBase dereferenced null pointer"));
			return (T&&)((Sub*)this)->Val;
		}

		// Allow dereferencing, if it's supported
		const T& operator->() const
		{
			solid_checkf(((Sub*)this)->Val, TEXT("TSolidNotNullBase dereferenced null pointer"));
			return ((Sub*)this)->Val;
		}
		
	}; // TSolidNotNullBase

	template <typename T>
	struct TSolidNotNullBase<T*>
	{
		using Sub = TSolidNotNull<T*>;

		// Allow implicit conversion to the wrapped value, for compatibility with non-TNotNull APIs
		FUNCTION_NON_NULL_RETURN_START
			operator T*() const
		FUNCTION_NON_NULL_RETURN_END
			{
				solid_checkf(((Sub*)this)->Val, TEXT("TSolidNotNullBase<T*> dereferenced null pointer"));
				return ((Sub*)this)->Val;
			}

		// Allow dereferencing, if it's supported
		FUNCTION_NON_NULL_RETURN_START
			T* operator->() const
		FUNCTION_NON_NULL_RETURN_END
			{
				solid_checkf(((Sub*)this)->Val, TEXT("TSolidNotNullBase<T*> dereferenced null pointer"));
				return ((Sub*)this)->Val;
			}
	};
	
#endif // #if DO_CHECK
	
} // namespace Solid::Private

/**
 * TSolidNotNull is a non-nullable, non-owning wrapper for pointer-like or nullable types.
 * It enforces an invariant that the contained value is never null. Checks occur at construction
 * and assignment via solid_checkf. In shipping builds, solid_checkf becomes a no-op but
 * [[assume]] hints are emitted for optimizer to assume non-null.
 */
template <typename T>
struct TSolidNotNull : public Solid::Private::TSolidNotNullBase<T>
{
	static_assert(!std::is_same_v<T, std::nullptr_t>, "TSolidNotNull cannot be instantiated with nullptr_t");
	static_assert(!std::is_same_v<T, void>, "TSolidNotNull cannot be instantiated with void");
	static_assert(!std::is_reference_v<T>, "TSolidNotNull cannot be instantiated with a reference type");
	static_assert(TIsPointer<T>::Value, "TSolidNotNull must be instantiated with a pointer-like type");

	friend struct Solid::Private::TSolidNotNullBase<T>;
	
private:
	T Val;
	
public:
	NO_DISCARD friend FORCEINLINE uint32 GetTypeHash(const TSolidNotNull& NotNull)
	{
		return GetTypeHash(NotNull.Val);
	}
	
	// Disallow default and nullptr construction/assignment
	TSolidNotNull() = delete;
	TSolidNotNull(std::nullptr_t) = delete;
	TSolidNotNull& operator=(std::nullptr_t) = delete;

	// Allow copy/move
	FORCEINLINE TSolidNotNull(const TSolidNotNull& Other)
		: Val(Other.Val)
	{
		solid_checkf(Val, TEXT("TSolidNotNull copy constructed from null value"));
	}
	
	FORCEINLINE TSolidNotNull(TSolidNotNull&& Other) noexcept
		: Val(Other.Val)
	{
		solid_checkf(Val, TEXT("TSolidNotNull move constructed from null value"));
	}
	
	FORCEINLINE TSolidNotNull& operator=(const TSolidNotNull& Other)
	{
		solid_checkf(Other.Val, TEXT("TSolidNotNull copy assigned from null value"));
		
		Val = Other.Val;
		return *this;
	}
	
	FORCEINLINE TSolidNotNull& operator=(TSolidNotNull&& Other) noexcept
	{
		solid_checkf(Other.Val, TEXT("TSolidNotNull move assigned from null value"));
		
		Val = Other.Val;
		return *this;
	}

	/**
	 * Construct from a value convertible to T. Immediately checks non-null via solid_checkf,
	 * then emits [[assume]] for the optimizer.
	 */
	template <typename U = T>
	requires(std::is_convertible_v<U, T> && !std::is_same_v<U, std::nullptr_t>)
	FORCEINLINE TSolidNotNull(U&& InVal)
		: Val((U&&)InVal)
	{
		solid_checkf(Val, TEXT("TSolidNotNull initialized with null value"));
	}
	
	FORCEINLINE operator T() const
	{
		solid_checkf(Val, TEXT("TSolidNotNull accessed with null value"));
		return Val;
	}

	/** Explicit getter */
	NO_DISCARD FORCEINLINE T Get() const
	{
		solid_checkf(Val, TEXT("TSolidNotNull accessed with null value"));
		return Val;
	}

	/**
	 * Disallow boolean testing of the wrapper itself.
	 * If you need to check the wrapped pointer, use Get() directly.
	 */
	explicit operator bool() const = delete;
	
	NO_DISCARD FORCEINLINE T* GetPtr() const
	{
		solid_checkf(Val, TEXT("TSolidNotNull accessed with null value"));
		return &Val;
	}

	FORCEINLINE bool operator==(const TSolidNotNull& Other) const
	{
		solid_checkf(Val && Other.Val, TEXT("TSolidNotNull comparison with null value"));
		return Val == Other.Val;
	}

	FORCEINLINE bool operator!=(const TSolidNotNull& Other) const
	{
		return !(*this == Other);
	}

	template <typename U>
	requires(std::is_convertible_v<U, T>)
	FORCEINLINE bool operator==(const U& Other) const
	{
		static_assert(!std::is_same_v<U, std::nullptr_t>, "Cannot compare TSolidNotNull with nullptr");
		
		solid_checkf(Val, TEXT("TSolidNotNull accessed with null value"));
		return Val == Other;
	}

	template <typename U>
	requires(std::is_convertible_v<U, T>)
	FORCEINLINE bool operator!=(const U& Other) const
	{
		static_assert(!std::is_same_v<U, std::nullptr_t>, "Cannot compare TSolidNotNull with nullptr");
		
		solid_checkf(Val, TEXT("TSolidNotNull accessed with null value"));
		return Val != Other;
	}
	
}; // class TSolidNotNull

#endif // ENABLE_SOLID_NOT_NULL