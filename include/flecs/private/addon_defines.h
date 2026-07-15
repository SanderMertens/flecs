/**
 * @file private/addon_defines.h
 * @brief Configure enabled addons and their dependencies.
 *
 * This file should only be included by the main flecs.h header.
 */

#ifndef FLECS_ADDON_DEFINES_H
#define FLECS_ADDON_DEFINES_H

/* Blacklist macros */
#ifdef FLECS_NO_CPP
#undef FLECS_CPP
#endif
#ifdef FLECS_NO_CACHED_QUERIES
#undef FLECS_CACHED_QUERIES
#endif
#ifdef FLECS_NO_CONSTRAINT_TRAITS
#undef FLECS_CONSTRAINT_TRAITS
#endif
#ifdef FLECS_NO_ENTITY_RANGES
#undef FLECS_ENTITY_RANGES
#endif
#ifdef FLECS_NO_FRAME
#undef FLECS_FRAME
#endif
#ifdef FLECS_NO_MODULE
#undef FLECS_MODULE
#endif
#ifdef FLECS_NO_MULTI_WORLD
#undef FLECS_MULTI_WORLD
#endif
#ifdef FLECS_NO_PREFAB
#undef FLECS_PREFAB
#endif
#ifdef FLECS_NO_SCRIPT
#undef FLECS_SCRIPT
#endif
#ifdef FLECS_NO_PARSER
#undef FLECS_PARSER
#endif
#ifdef FLECS_NO_QUERY_DSL
#undef FLECS_QUERY_DSL
#endif
#ifdef FLECS_NO_SCRIPT_MATH
#undef FLECS_SCRIPT_MATH
#endif
#ifdef FLECS_NO_SCRIPT_PLATFORM
#undef FLECS_SCRIPT_PLATFORM
#endif
#ifdef FLECS_NO_STATS
#undef FLECS_STATS
#endif
#ifdef FLECS_NO_SYSTEM
#undef FLECS_SYSTEM
#endif
#ifdef FLECS_NO_ALERTS
#undef FLECS_ALERTS
#endif
#ifdef FLECS_NO_PIPELINE
#undef FLECS_PIPELINE
#endif
#ifdef FLECS_NO_TIMER
#undef FLECS_TIMER
#endif
#ifdef FLECS_NO_META
#undef FLECS_META
#endif
#ifdef FLECS_NO_METRICS
#undef FLECS_METRICS
#endif
#ifdef FLECS_NO_UNITS
#undef FLECS_UNITS
#endif
#ifdef FLECS_NO_JSON
#undef FLECS_JSON
#endif
#ifdef FLECS_NO_DOC
#undef FLECS_DOC
#endif
#ifdef FLECS_NO_LOG
#undef FLECS_LOG
#endif
#ifdef FLECS_NO_APP
#undef FLECS_APP
#endif
#ifdef FLECS_NO_OS_API_IMPL
#undef FLECS_OS_API_IMPL
#endif
#ifdef FLECS_NO_HTTP
#undef FLECS_HTTP
#endif
#ifdef FLECS_NO_REST
#undef FLECS_REST
#endif
#ifdef FLECS_NO_JOURNAL
#undef FLECS_JOURNAL
#endif

/* Resolve addon dependencies before addon-dependent API declarations. The
 * order of these blocks follows the addon dependency graph, from addons with
 * the most dependencies to addons with the least dependencies. */
#if defined(FLECS_SCRIPT_MATH) || defined(FLECS_SCRIPT_PLATFORM)
#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif
#endif

#ifdef FLECS_REST
#ifndef FLECS_HTTP
#define FLECS_HTTP
#endif
#ifndef FLECS_JSON
#define FLECS_JSON
#endif
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif
#endif

#if defined(FLECS_ALERTS) || defined(FLECS_APP) || defined(FLECS_TIMER)
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif
#endif

#ifdef FLECS_STATS
#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif
#ifndef FLECS_TIMER
#define FLECS_TIMER
#endif
#endif

#ifdef FLECS_METRICS
#ifndef FLECS_META
#define FLECS_META
#endif
#ifndef FLECS_UNITS
#define FLECS_UNITS
#endif
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif
#endif

#ifdef FLECS_JSON
#ifndef FLECS_META
#define FLECS_META
#endif
#ifndef FLECS_DOC
#define FLECS_DOC
#endif
#ifndef FLECS_QUERY_DSL
#define FLECS_QUERY_DSL
#endif
#endif

#ifdef FLECS_SCRIPT
#ifndef FLECS_META
#define FLECS_META
#endif
#ifndef FLECS_DOC
#define FLECS_DOC
#endif
#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif
#endif

#ifdef FLECS_UNITS
#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif
#ifndef FLECS_META
#define FLECS_META
#endif
#endif

#ifdef FLECS_PIPELINE
#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif
#ifndef FLECS_SYSTEM
#define FLECS_SYSTEM
#endif
#ifndef FLECS_CACHED_QUERIES
#define FLECS_CACHED_QUERIES
#endif
#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif
#endif

#if defined(FLECS_DOC) || defined(FLECS_META) || defined(FLECS_SYSTEM) || \
    defined(FLECS_TIMER)
#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif
#endif

#ifdef FLECS_QUERY_DSL
#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif
#endif

#if defined(FLECS_ALERTS) || defined(FLECS_APP) || defined(FLECS_HTTP) || \
    defined(FLECS_JSON) || defined(FLECS_METRICS) || defined(FLECS_PIPELINE) || \
    defined(FLECS_REST) || defined(FLECS_STATS) || defined(FLECS_SYSTEM) || \
    defined(FLECS_TIMER)
#ifndef FLECS_FRAME
#define FLECS_FRAME
#endif
#endif

#if defined(FLECS_FRAME) || defined(FLECS_HTTP)
#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif
#endif

#endif
