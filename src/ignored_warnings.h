/**
 * @file ignored_warnings.h
 * @brief Warnings ignored when compiling Flecs sources.
 */

#ifndef FLECS_IGNORED_WARNINGS_H
#define FLECS_IGNORED_WARNINGS_H

/* Ignored warnings */
#if defined(ECS_TARGET_CLANG)
/* Ignore unknown options so we don't have to care about the compiler version. */
#pragma clang diagnostic ignored "-Wunknown-warning-option"
/* Warns for double or redundant semicolons. There are legitimate cases where a
 * semicolon after an empty statement is useful, for example, after a macro that
 * is replaced with a code block. With this warning enabled, semicolons would 
 * only have to be added after macros that are not code blocks, which in some
 * cases isn't possible as the implementation of a macro can be different in
 * debug/release mode. */
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
/* This is valid in C99, and Flecs must be compiled as C99. */
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
/* Clang attribute to detect fallthrough isn't supported on older versions. 
 * Implicit fallthrough is still detected by gcc and ignored with "fall through"
 * comments. */
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
/* This warning prevents adding a default case when all enum constants are part
 * of the switch. In C, however, an enum type can assume any value in the range of
 * the type, and this warning makes it harder to catch invalid enum values. */
#pragma clang diagnostic ignored "-Wcovered-switch-default"
/* This warning prevents some casts of function results to a different kind of
 * type, e.g., casting an int result to double. Not very useful in practice, as
 * it just forces the code to assign to a variable first, then cast. */
#pragma clang diagnostic ignored "-Wbad-function-cast"
/* Format strings can be passed down from other functions. */
#pragma clang diagnostic ignored "-Wformat-nonliteral"
/* Useful, but not reliable enough. It can incorrectly flag macros as unused
 * in standalone builds. */
#pragma clang diagnostic ignored "-Wunused-macros"
/* This warning gets thrown by clang even when the code is handling all case
 * values but not all cases (for example, when the switch contains a LastValue
 * case). Adding a "default" case fixes the warning, but silences future 
 * warnings about unhandled cases, which is worse. */
#pragma clang diagnostic ignored "-Wswitch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#if __clang_major__ == 13
/* clang 13 can throw this warning for a macro in ctype.h. */
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
/* Filenames aren't consistent across targets as they can use different casing 
 * (e.g., WinSock2 vs. winsock2). */
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
/* Very difficult to work around this warning in C, especially for an ECS. */
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
/* This warning gets thrown when trying to cast a pointer returned from dlproc. */
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
/* This warning can get thrown for expressions that evaluate to constants
 * in debug/release mode. */
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
/* With soft asserts enabled, the code won't abort, which in some cases means
 * code paths are reached where values are uninitialized. */
#ifdef FLECS_SOFT_ASSERT
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#endif

/* Allows for enum reflection support on legacy compilers. */
#if __clang_major__ < 16
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif

#elif defined(ECS_TARGET_GNU)
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"
#pragma GCC diagnostic ignored "-Wbad-function-cast"
#endif
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wswitch-enum"
/* This warning gets thrown *sometimes* when not all members for a struct are
 * provided in an initializer. Flecs heavily relies on descriptor structs that
 * only require partial initialization, so this warning isn't useful.
 * It doesn't introduce any safety issues (fields are guaranteed to be 0 
 * initialized), and later versions of gcc (>=11) seem to no longer throw this 
 * warning. */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
/* Produces false positives in addons/cpp/delegate.hpp. */
#pragma GCC diagnostic ignored "-Warray-bounds"
/* Produces false positives in queries/src/cache.c. */
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wrestrict"

#elif defined(ECS_TARGET_MSVC)
/* Recursive on all control paths, the function will cause runtime stack overflow.
 * This warning is incorrectly thrown on enum reflection code. */
#pragma warning(disable: 4717)
#endif

/* Allows for enum reflection support on legacy compilers. */
#if defined(__GNUC__) && __GNUC__ <= 10
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#endif
