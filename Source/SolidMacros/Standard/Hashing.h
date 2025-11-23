// Solstice Games © 2024. All Rights Reserved.

#ifndef SOLID_MACROS_STANDARD_HASHING_H
#define SOLID_MACROS_STANDARD_HASHING_H

#include <vector>
#include <functional>

#include "CoreMinimal.h"

#include "UObject/ObjectKey.h"
#include "GameplayTagsManager.h"

#include "SolidMacros/Macros.h"

namespace Solid
{
	template <uint32 Count = 2>
	static NO_DISCARD uint32 HashCombine(const uint32 (&Hashes)[Count])
	{
		uint32 Hash = 2166136261U;
		
		for (uint32 Index = 0; Index < Count; ++Index)
		{
			Hash ^= Hashes[Index];
			Hash *= 16777619U;
		}

		return Hash;
	}
	
} // namespace Solid

#define DEFINE_STD_HASH(x) \
	template <> \
	struct std::hash<x> \
	{ \
	public: \
		SOLID_INLINE std::size_t operator()(const x& Value) const NOEXCEPT \
		{ \
			return GetTypeHash(Value); \
		} \
		\
	}; // struct std::hash<x>

#define DEFINE_STD_HASH_CUSTOM_FUNC(x, FUNC) \
	template <> \
	struct std::hash<x> \
	{ \
	public: \
		SOLID_INLINE std::size_t operator()(const x& Value) const NOEXCEPT \
		{ \
			return FUNC(Value); \
		} \
		\
	}; // struct std::hash<x>

#define DEFINE_STD_HASH_TEMPLATED(x, ...) \
	template <##__VA_ARGS__> \
	struct std::hash<x<##__VA_ARGS__>> \
	{ \
	public: \
		SOLID_INLINE std::size_t operator()(const x<T>& Value) const NOEXCEPT \
		{ \
			return GetTypeHash(Value); \
		} \
		\
	}; // struct std::hash<x<T>>

#define DEFINE_STD_HASH_TEMPLATED_CUSTOM_FUNC(x, FUNC, ...) \
	template <__VA_ARGS__> \
	struct std::hash<x<__VA_ARGS__>> \
	{ \
	public: \
		SOLID_INLINE std::size_t operator()(const x<T>& Value) const NOEXCEPT \
		{ \
			return FUNC(Value); \
		} \
		\
	}; // struct std::hash<x<T>>

DEFINE_STD_HASH(FName)
DEFINE_STD_HASH(FString);
DEFINE_STD_HASH(FStringView);
DEFINE_STD_HASH(FGameplayTag);


template <typename T>
struct std::hash<TObjectKey<typename T>>
{
public:
	SOLID_INLINE std::size_t operator()(const TObjectKey<T>& Value) const NOEXCEPT
	{
		return GetTypeHash(Value);
	}
	
};; // struct std::hash<TObjectKey<T>>

/*template <typename T>
struct std::hash<TObjectKey<typename T>>
{
public:
	SOLID_INLINE std::size_t operator()(const TObjectKey<T>& Value) const NOEXCEPT
	{
		return GetTypeHash(Value);
	}
};;*/

#endif // SOLID_MACROS_STANDARD_HASHING_H