// Solstice Games © 2023. All Rights Reserved.

#pragma once

#ifndef SOLID_MACROS_H
#define SOLID_MACROS_H

#include <algorithm>
#include <array>
#include <codecvt>
#include <string_view>
#include <utility>

#include "CoreMinimal.h"

#include "Misc/AssertionMacros.h"
#include "GenericPlatform/GenericPlatformTime.h"

namespace Solid
{
	constexpr std::string_view strip_prefix(std::string_view name)
	{
		constexpr std::array<const char*, 3> prefixes = {"struct ", "class ", "enum "};
		for (const auto* prefix : prefixes)
		{
			std::string_view prefix_sv(prefix);
			if (name.starts_with(prefix_sv))
			{
				return name.substr(prefix_sv.size());
			}
		}
		return name;
	}

	template <std::size_t ...Idxs>
	FORCEINLINE constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
	{
		return std::array{str[Idxs]..., '\n'};
	}

	template <typename T>
	FORCEINLINE constexpr auto type_name_array()
	{
		#if defined(__clang__)
		constexpr auto prefix   = std::string_view{"[T = "};
		constexpr auto suffix   = std::string_view{"]"};
		constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
		#elif defined(__GNUC__) // defined(__clang__)
		constexpr auto prefix   = std::string_view{"with T = "};
		constexpr auto suffix   = std::string_view{"]"};
		constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
		#elif defined(_MSC_VER) // defined(__GNUC__)
		constexpr auto prefix   = std::string_view{"type_name_array<"};
		constexpr auto suffix   = std::string_view{">(void)"};
		constexpr auto function = std::string_view{__FUNCSIG__};
		#else // #if defined(_MSC_VER)
		#error Unsupported compiler
		#endif // #else defined(_MSC_VER)

		constexpr size_t start = function.find(prefix) + prefix.size();
		constexpr size_t end = function.rfind(suffix);

		static_assert(start < end, "Failed to parse type name.");

		constexpr std::string_view name = function.substr(start, (end - start));
		constexpr std::string_view stripped_name = strip_prefix(name);
		return substring_as_array(stripped_name, std::make_index_sequence<stripped_name.size()>{});
	}

	template <typename T>
	struct type_name_holder
	{
		static inline constexpr auto value = type_name_array<T>();
	}; // struct type_name_holder

	template <typename T>
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	constexpr auto type_name() -> std::string_view
	{
		constexpr auto& value = type_name_holder<T>::value;
		return std::string_view{value.data(), value.size()};
	}

	namespace internal
	{
		template <typename... T>
		FORCEINLINE const TCHAR* BuildCheckTextFormat(const TCHAR* Format, T&&... Args)
		{
			FStringFormatOrderedArguments OrderedArgs;
			OrderedArgs.Reserve(sizeof...(T));
			
			(OrderedArgs.Emplace(Forward<T>(Args)), ...);
			
			FString Buffer;
			Buffer = FString::Format(Format, OrderedArgs);

			return *Buffer;
		}
		
	} // namespace internal
	
} // namespace Solid

#endif // SOLID_MACROS_H

#ifndef HAS_CPP_ATTRIBUTE
#define HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif // HAS_CPP_ATTRIBUTE

#ifndef EXPECTS

#if HAS_CPP_ATTRIBUTE(expects)

#define EXPECTS(x) [[expects : x]]

#else // #if HAS_CPP_ATTRIBUTE(expects)

#define EXPECTS(x)

#endif // #else HAS_CPP_ATTRIBUTE(expects)

#endif // EXPECTS

#ifndef ENSURES

#if HAS_CPP_ATTRIBUTE(ensures)

#define ENSURES(x) [[ensures : x]]

#else // #if HAS_CPP_ATTRIBUTE(ensures)

#define ENSURES(x)

#endif // #else HAS_CPP_ATTRIBUTE(ensures)

#endif // ENSURES

#ifndef CLANG_ATTRIBUTE
#define CLANG_ATTRIBUTE __attribute__
#endif // CLANG_ATTRIBUTE

#ifndef IS_CLANG

#if defined(__clang__)

#define IS_CLANG 1

#else // #if defined(__clang__)

#define IS_CLANG 0

#endif // #else defined(__clang__)

#endif // IS_CLANG

#ifndef IS_GNU

#if defined(__GNUC__)

#define IS_GNU 1

#else // #if defined(__GNUC__)

#define IS_GNU 0

#endif // #else defined(__GNUC__)

#endif // IS_GNU

#ifndef IS_MSVC

#if defined(_MSC_VER)

#define IS_MSVC 1

#else // defined(_MSC_VER)

#define IS_MSVC 0

#endif // defined(_MSC_VER

#endif // IS_MSVC

#ifndef IS_GCC
#define IS_GCC IS_GNU
#endif // IS_GCC

#ifndef IS_UNIX
#define IS_UNIX PLATFORM_UNIX
#endif // IS_UNIX

#ifndef IS_WINDOWS
#define IS_WINDOWS PLATFORM_WINDOWS
#endif // IS_WINDOWS

#ifndef IS_LINUX
#define IS_LINUX PLATFORM_LINUX
#endif // IS_LINUX

#ifndef DECLSPEC

	#if IS_MSVC

	#define DECLSPEC(Expr) __declspec(Expr)

	#else // IS_MSVC

	#define DECLSPEC(Expr)

	#endif // IS_MSVC

#endif // DECLSPEC

#ifndef NAKED
#define NAKED DECLSPEC(naked)
#endif // NAKED

#ifndef FORCEINLINE_CALLS
#define FORCEINLINE_CALLS [[msvc::forceinline_calls]]
#endif // FORCEINLINE_CALLS

#ifndef FORCEINLINE_ATTRIBUTE

#if IS_MSVC

#define FORCEINLINE_ATTRIBUTE [[msvc::forceinline]]

#elif IS_GNU || IS_CLANG

#define FORCEINLINE_ATTRIBUTE __attribute__((always_inline))

#else // IS_GNU || IS_CLANG

#define FORCEINLINE_ATTRIBUTE

#endif // IS_MSVC

#endif // FORCEINLINE_ATTRIBUTE

#ifndef LIKELY_ATTRIBUTE
#define LIKELY_ATTRIBUTE [[likely]]
#endif // LIKELY_ATTRIBUTE

#ifndef LIKELY_IF
#define LIKELY_IF(x) (x) LIKELY_ATTRIBUTE
#endif // LIKELY_IF

#ifndef UNLIKELY_ATTRIBUTE
#define UNLIKELY_ATTRIBUTE [[unlikely]]
#endif // UNLIKELY_ATTRIBUTE

#ifndef UNLIKELY_IF
#define UNLIKELY_IF(x) (x) UNLIKELY_ATTRIBUTE
#endif // UNLIKELY_IF

#ifndef CPP_VERSION
#define CPP_VERSION __cplusplus
#endif // CPP_VERSION

#ifndef CPP_VERSION_98
#define CPP_VERSION_98 199711L
#endif // CPP_VERSION_98

#ifndef CPP_VERSION_11
#define CPP_VERSION_11 201103L
#endif // CPP_VERSION_11

#ifndef CPP_VERSION_14
#define CPP_VERSION_14 201402L
#endif // CPP_VERSION_14

#ifndef CPP_VERSION_17
#define CPP_VERSION_17 201703L
#endif // CPP_VERSION_17

#ifndef CPP_VERSION_20
#define CPP_VERSION_20 202002L
#endif // CPP_VERSION_20

#ifndef CPP_VERSION_23
#define CPP_VERSION_23 202300L
#endif // CPP_VERSION_23

#ifndef OPTIONAL_FORCEINLINE
#define OPTIONAL_FORCEINLINE inline
#endif // OPTIONAL_FORCEINLINE

#ifdef LIKELY
#undef LIKELY
#endif // LIKELY

#ifdef UNLIKELY
#undef UNLIKELY
#endif // UNLIKELY

#ifndef LIKELY

#if IS_MSVC

#define LIKELY(x) (!!(x))

#elif IS_CLANG || IS_GNU

#define LIKELY(x) __builtin_expect(!!(x), 1)

#else // IS_MSVC

#define LIKELY(x) (!!(x))

#endif // IS_MSVC

#endif // LIKELY

#ifndef UNLIKELY

#if IS_MSVC

#define UNLIKELY(x) (!!(x))

#elif IS_CLANG || IS_GNU

#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#else // IS_MSVC

#define UNLIKELY(x) (!!(x))

#endif // IS_MSVC

#endif // UNLIKELY

#ifndef SOLID_INLINE

#if !UE_BUILD_SHIPPING

#define SOLID_INLINE inline

#else // WITH_EDITOR || !UE_BUILD_SHIPPING

#define SOLID_INLINE FORCEINLINE

#endif // WITH_EDITOR || !UE_BUILD_SHIPPING

#endif // SOLID_INLINE

#ifndef NO_DISCARD
#define NO_DISCARD [[nodiscard]]
#endif // NO_DISCARD

#ifndef NO_DISCARD_REASON
#define NO_DISCARD_REASON(x) [[nodiscard(x)]]
#endif // NO_DISCARD_REASON

#ifndef NO_UNIQUE_ADDRESS
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif // NO_UNIQUE_ADDRESS

#ifndef NO_VTABLE

#if HAS_CPP_ATTRIBUTE(no_vtable)

#define NO_VTABLE [[no_vtable]]

#elif IS_MSVC

#define NO_VTABLE DECLSPEC(novtable)

#else // IS_MSVC

#define NO_VTABLE

#endif // HAS_CPP_ATTRIBUTE(no_vtable)

#endif // NO_VTABLE

#ifndef NO_THROW

#if HAS_CPP_ATTRIBUTE(nothrow)

#define NO_THROW [[nothrow]]

#elif IS_MSVC

#define NO_THROW __declspec(nothrow)

#else // IS_MSVC

#define NO_THROW

#endif // HAS_CPP_ATTRIBUTE(nothrow)

#endif // NO_THROW

#ifndef NO_ALIAS

#if HAS_CPP_ATTRIBUTE(noalias)
#define NO_ALIAS [[noalias]]
#elif IS_MSVC
#define NO_ALIAS __declspec(noalias)
#else // IS_MSVC
#define NO_ALIAS
#endif // IS_MSVC

#endif // NO_ALIAS

#ifndef MAYBE_UNUSED
#define MAYBE_UNUSED [[maybe_unused]]
#endif // MAYBE_UNUSED

#ifndef DEPRECATED
#define DEPRECATED [[deprecated]]
#endif // DEPRECATED

#ifndef DEPRECATED_REASON
#define DEPRECATED_REASON(x) [[deprecated(x)]]
#endif // DEPRECATED_REASON

#ifndef NO_INLINE
	#if IS_MSVC
	#define NO_INLINE DECLSPEC(noinline)
	#elif IS_GNU || IS_CLANG
	#define NO_INLINE __attribute__((noinline))
	#else // IS_GNU || IS_CLANG
	#define NO_INLINE
	#endif // IS_MSVC
#endif // NO_INLINE

#ifndef NO_GUARD
#define NO_GUARD [[msvc::no_tls_guard]]
#endif // NO_GUARD

#ifndef NO_RETURN
#define NO_RETURN [[noreturn]]
#endif // NO_RETURN

#ifndef NO_SANITIZE
	#if HAS_CPP_ATTRIBUTE(no_sanitize)
	#define NO_SANITIZE(x) [[no_sanitize(x)]]
	#elif IS_GNU || IS_CLANG
	#define NO_SANITIZE(x) CLANG_ATTRIBUTE((no_sanitize(x)))
	#else // IS_GNU || IS_CLANG
	#define NO_SANITIZE(x)
	#endif // HAS_CPP_ATTRIBUTE(no_sanitize)
#endif // NO_SANITIZE

#ifndef NOEXCEPT
#define NOEXCEPT noexcept
#endif // NOEXCEPT

#ifndef NO_EXCEPT
#define NO_EXCEPT NOEXCEPT
#endif // NO_EXCEPT

#ifndef nameof
#define nameof(x) Solid::type_name<x>()
#endif // nameof

#ifndef NAME_OF
#define NAME_OF(x) nameof(x)
#endif // NAME_OF

// Generic Output: "FunctionName"
#ifndef FUNCTION_NAME

#if defined(__GNUC__) || defined(__clang__)
#define FUNCTION_NAME __func__
#elif IS_MSVC
#define FUNCTION_NAME __FUNCTION__
#else // defined(__GNUC__) || defined(__clang__)
#error Unsupported compiler
#endif // defined(__GNUC__) || defined(__clang__)

#endif // FUNCTION_NAME

// Generic Output: "void __cdecl FunctionName(int, float)"
#ifndef FUNCTION_SIGNATURE
#define FUNCTION_SIGNATURE __FUNCSIG__
#endif // FUNCTION_SIGNATURE

#ifndef FUNCTION_SIG
#define FUNCTION_SIG FUNCTION_SIGNATURE
#endif // FUNCTION_SIG

#ifndef FUNC_SIG
#define FUNC_SIG FUNCTION_SIGNATURE
#endif // FUNC_SIG

#ifndef FUNC_SIGNATURE
#define FUNC_SIGNATURE FUNCTION_SIGNATURE
#endif // FUNC_SIGNATURE

#ifndef FUNC_NAME
#define FUNC_NAME FUNCTION_NAME
#endif // FUNC_NAME

#ifndef ASSUME

#if HAS_CPP_ATTRIBUTE(assume)

#define SOLID_ASSUME(x) [[assume(x)]]

#else // HAS_CPP_ATTRIBUTE(assume)

#define SOLID_ASSUME(x) UE_ASSUME(x)

#endif // HAS_CPP_ATTRIBUTE(assume)

#endif // ASSUME

#ifndef FALLTHROUGH
#define FALLTHROUGH _FALLTHROUGH
#endif // FALLTHROUGH

#ifndef OPTIMIZE_FOR_SYNC
#define OPTIMIZE_FOR_SYNC [[optimize_for_synchronized]]
#endif // OPTIMIZE_FOR_SYNC

#ifndef CARRIES_DEPENDENCY
#define CARRIES_DEPENDENCY [[carries_dependency]]
#endif // CARRIES_DEPENDENCY

#ifndef FLATTEN

	#if IS_MSVC

	#define FLATTEN [[msvc::flatten]]

	#elif IS_GNU || IS_CLANG

	#define FLATTEN __attribute__((flatten))

	#else // IS_GNU || IS_CLANG

	#define FLATTEN

	#endif // IS_MSVC

#endif // FLATTEN

#ifndef INTRINSIC
#define INTRINSIC [[msvc::intrinsic]]
#endif // INTRINSIC

#ifndef FILE_NAME
#define FILE_NAME __FILE__
#endif // FILE_NAME

#ifndef LINE_NUMBER
#define LINE_NUMBER __LINE__
#endif // LINE_NUMBER

#ifndef TIME_STAMP
#define TIME_STAMP __TIME__
#endif // TIME_STAMP

#ifndef COUNTER
#define COUNTER __COUNTER__
#endif // COUNTER

#if defined(__GNUC__) // JETBRAINS_IDE

#ifndef FORMAT
#define FORMAT [[gnu::format]]
#endif // FORMAT

#elif defined(__clang__) // defined(__GNUC__)

#ifndef FORMAT
#define FORMAT [[clang::format]]
#endif // FORMAT

#else // defined(__clang__)

#ifndef FORMAT
#define FORMAT
#endif // FORMAT

#endif // JETBRAINS_IDE

#ifndef JETBRAINS_GUARD
#define JETBRAINS_GUARD [[jetbrains::guard]]
#endif // JETBRAINS_GUARD

#ifndef JETBRAINS_PASS_BY_VALUE
#define JETBRAINS_PASS_BY_VALUE [[jetbrains::pass_by_value]]
#endif // JETBRAINS_PASS_BY_VALUE

#ifndef UNREACHABLE

#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE check(!"Unreachable"); __builtin_unreachable();
#else // defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE check(!"Unreachable"); UE_ASSUME(false);
#endif // defined(__GNUC__) || defined(__clang__)

#endif // UNREACHABLE

#ifndef BINARY_LITERAL
#define BINARY_LITERAL(x) 0b##x
#endif // BINARY_LITERAL

#ifndef DEFINE_CUSTOM_LITERAL
#define DEFINE_CUSTOM_LITERAL(Name, Type, Literal) \
	constexpr Type operator"" Name(unsigned long long int Value) \
	{ \
		return static_cast<Type>(Value); \
	}
#endif // DEFINE_CUSTOM_LITERAL

#ifndef EXISTS
#define EXISTS(x) (!std::is_same_v<decltype(x), void>)
#endif // EXISTS

#ifndef NOT_EXISTS
#define NOT_EXISTS(x) (std::is_same_v<decltype(x), void>)
#endif // NOT_EXISTS

#ifndef IF_EXISTS
#define IF_EXISTS(x) if constexpr EXISTS(x)
#endif // IF_EXISTS

#ifndef IF_NOT_EXISTS
#define IF_NOT_EXISTS(x) if constexpr NOT_EXISTS(x)
#endif // IF_NOT_EXISTS

#ifndef UNDERLYING_TYPE
#define UNDERLYING_TYPE(x) std::underlying_type_t<x>
#endif // UNDERLYING_TYPE

#ifndef FUNCTION_TEXT

#if defined(_MSC_VER)
// On MSVC __FUNCTION__ is a compile‐time literal so we can do literal concatenation.
#define FUNCTION_TEXT(Message) TEXT(__FUNCTION__) TEXT(": ") TEXT(Message)
#else
// On other compilers (like Clang/GCC), __func__ is not a preprocessor literal.
// In this case, use the non-Printf (ordered arguments) version of the log macro.
#define FUNCTION_TEXT(Message) (FString(ANSI_TO_TCHAR(__func__)) + TEXT(": ") + FString(TEXT(Message)))
#endif

#endif // FUNCTION_TEXT

#ifndef MSVC_WARNING_PUSH
#define MSVC_WARNING_PUSH __pragma(warning(push))
#endif // MSVC_WARNING_PUSH

#ifndef MSVC_WARNING_POP
#define MSVC_WARNING_POP __pragma(warning(pop))
#endif // MSVC_WARNING_POP

#ifndef MSVC_WARNING_DISABLE
#define MSVC_WARNING_DISABLE(x) __pragma(warning(disable : x))
#endif // MSVC_WARNING_DISABLE

#ifndef HOT_CODE_PATH
	#if IS_CLANG
	#define HOT_CODE_PATH CLANG_ATTRIBUTE(hot)
	#else // IS_CLANG
	#define HOT_CODE_PATH 
	#endif // IS_CLANG
#endif // HOT_CODE_PATH

#ifndef COLD_CODE_PATH
	#if IS_CLANG
	#define COLD_CODE_PATH CLANG_ATTRIBUTE((cold))
	#else // IS_CLANG
	#define COLD_CODE_PATH
	#endif // IS_CLANG
#endif // COLD_CODE_PATH

/*namespace Solid::detail
{
	template<uint32 NumRuns, typename TestT>
	void SolidBenchmark(const TCHAR* TestName, TestT&& TestBody)
	{
		UE_LOG(LogTemp, Log, TEXT("\n-------------------------------\n%s"), TestName);
		double MinTime = TNumericLimits<double>::Max();
		double TotalTime = 0;
	
		for (uint32 RunNo = 0; RunNo != NumRuns; ++RunNo)
		{
			double Time = FPlatformTime::Seconds();
			TestBody();
			Time = FPlatformTime::Seconds() - Time;

			const double Ms = (Time * 1000);

			UE_LOG(LogTemp, Warning, TEXT("#%d: %f secs (%f ms)"), RunNo, Time, Ms);

			TotalTime += Time;

			MinTime = std::min(MinTime, Time);
		}

		const double AverageTimeMs = (TotalTime / NumRuns) * 1000;
		const double MinTimeMs = MinTime * 1000;
	
		UE_LOG(LogTemp, Warning,
			TEXT("min: %f secs (%d ms), avg: %f secs (%f ms)\n-------------------------------\n"),
			MinTime, static_cast<uint32>(MinTimeMs), TotalTime / NumRuns, AverageTimeMs);
		UE_LOG(LogTemp, Warning,
			TEXT("Microseconds: min: %f, avg: %f\n-------------------------------\n"),
			MinTime * 1000000, (TotalTime / NumRuns) * 1000000);
	}
	
} // namespace Solid::detail

#ifndef SOLID_BENCHMARK
#define SOLID_BENCHMARK(NumRuns, ...) \
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT(#__VA_ARGS__)); \
	Solid::detail::SolidBenchmark<NumRuns>(TEXT(#__VA_ARGS__),
#endif // SOLID_BENCHMARK*/

#ifndef solid_check

#if !UE_BUILD_SHIPPING || USE_CHECKS_IN_SHIPPING

#define solid_check(expr) \
	check(expr)

#define solid_checkf(expr, format, ...) \
	checkf(expr, format, ##__VA_ARGS__)

#define solid_checkfmt(expr, format, ...) \
	do \
	{ \
		UE_CHECK_F_IMPL(expr, TEXT("%s"), Solid::internal::BuildCheckTextFormat(format, ##__VA_ARGS__)); \
	} while (false)

#define solid_ensure(expr) ensure(expr)
#define solid_ensureMsgf(expr, format, ...) ensureMsgf(expr, format, ##__VA_ARGS__)

#define solid_ensurefmt(expr, format, ...) \
	UE_ENSURE_F_IMPL(expr, TEXT("%s"), Solid::internal::BuildCheckTextFormat(format, ##__VA_ARGS__))

#define solid_verify(expr) verify(expr)
#define solid_verifyf(expr, format, ...) verifyf(expr, format, ##__VA_ARGS__)

#define solid_verifyfmt(expr, format, ...) \
	do \
	{ \
		UE_CHECK_F_IMPL(expr, TEXT("%s"), Solid::internal::BuildCheckTextFormat(format, ##__VA_ARGS__)); \
	} while (false)

// Compiles down to an assume in shipping builds
#define solid_cassume(expr) check(expr)

// Compiles down to an assume in shipping builds
#define solid_cassumef(expr, format, ...) checkf(expr, format, ##__VA_ARGS__)

#define solid_cassumefmt(expr, format, ...) \
	do \
	{ \
		UE_CHECK_F_IMPL(expr, TEXT("%s"), Solid::internal::BuildCheckTextFormat(format, ##__VA_ARGS__)); \
	} while (false)

#else // UE_BUILD_SHIPPING || USE_CHECKS_IN_SHIPPING

#define solid_check(expr) check(false)
#define solid_checkf(expr, format, ...) checkf(false, format, ##__VA_ARGS__)
#define solid_checkfmt(expr, format, ...) UE_EMPTY

#define solid_ensure(expr)
#define solid_ensuref(expr, format, ...)
#define solid_ensurefmt(expr, format, ...) UE_EMPTY

#define solid_verify(expr) { if UNLIKELY_IF(!(expr)) { ASSUME(false); } }
#define solid_verifyf(expr, format, ...) { if UNLIKELY_IF(!(expr)) { ASSUME(false); } }
#define solid_verifyfmt(expr, format, ...) if UNLIKELY_IF(!(expr)) { ASSUME(false); }

#define solid_cassume(expr) SOLID_ASSUME(expr)
#define solid_cassumef(expr, format, ...) SOLID_ASSUME(expr)
#define solid_cassumefmt(expr, format, ...) UE_EMPTY

#endif // UE_BUILD_SHIPPING || USE_CHECKS_IN_SHIPPING

#endif // solid_check

#ifndef SOLID_RESTRICT
	#if defined(__cplusplus)
	#if defined(__GNUC__) || defined(__clang__)
	#define SOLID_RESTRICT __restrict__
	#elif defined(_MSC_VER) // defined(__GNUC__) || defined(__clang__)
	#define SOLID_RESTRICT __restrict
	#else // defined(_MSC_VER)
	#define SOLID_RESTRICT
	#endif // defined(_MSC_VER)
	#else // defined(__cplusplus)
	#define SOLID_RESTRICT restrict
	#endif // defined(__cplusplus)
#endif // SOLID_RESTRICT

#ifndef SOLID_SELECT_ANY
#define SOLID_SELECT_ANY UE_SELECT_ANY
#endif // SOLID_SELECT_ANY
