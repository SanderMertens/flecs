/**
 * @file addons.h
 * @brief Include enabled addons.
 *
 * This file should only be included by the main flecs.h header.
 */

#ifndef FLECS_ADDONS_H
#define FLECS_ADDONS_H

/* Don't enable web addons if we're running as a webasm app */
#ifdef ECS_TARGET_EM
#ifndef FLECS_NO_HTTP
#define FLECS_NO_HTTP
#endif // FLECS_NO_HTTP

#ifndef FLECS_NO_REST
#define FLECS_NO_REST
#endif // FLECS_NO_REST
#endif // ECS_TARGET_EM

/* Blacklist macros */
#ifdef FLECS_NO_CPP
#undef FLECS_CPP
#endif
#ifdef FLECS_NO_MODULE
#undef FLECS_MODULE
#endif
#ifdef FLECS_NO_PARSER
#undef FLECS_PARSER
#endif
#ifdef FLECS_NO_PLECS
#undef FLECS_PLECS
#endif
#ifdef FLECS_NO_RULES
#undef FLECS_RULES
#endif
#ifdef FLECS_NO_SNAPSHOT
#undef FLECS_SNAPSHOT
#endif
#ifdef FLECS_NO_MONITOR
#undef FLECS_MONITOR
#endif
#ifdef FLECS_NO_STATS
#undef FLECS_STATS
#endif
#ifdef FLECS_NO_SYSTEM
#undef FLECS_SYSTEM
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
#ifdef FLECS_NO_META_C
#undef FLECS_META_C
#endif
#ifdef FLECS_NO_UNITS
#undef FLECS_UNITS
#endif
#ifdef FLECS_NO_EXPR
#undef FLECS_EXPR
#endif
#ifdef FLECS_NO_JSON
#undef FLECS_JSON
#endif
#ifdef FLECS_NO_DOC
#undef FLECS_DOC
#endif
#ifdef FLECS_NO_COREDOC
#undef FLECS_COREDOC
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

/* Always included, if disabled functions are replaced with dummy macros */
#include "flecs/addons/journal.h"
#include "flecs/addons/log.h"

#ifdef FLECS_MONITOR
#ifndef FLECS_STATS
#define FLECS_STATS
#endif
#ifndef FLECS_SYSTEM
#define FLECS_SYSTEM
#endif
#ifndef FLECS_TIMER
#define FLECS_TIMER
#endif
#endif

#ifdef FLECS_APP
#ifdef FLECS_NO_APP
#error "FLECS_NO_APP failed: APP is required by other addons"
#endif
#include "../addons/app.h"
#endif
#ifdef FLECS_REST
#ifdef FLECS_NO_REST
#error "FLECS_NO_REST failed: REST is required by other addons"
#endif
#include "../addons/rest.h"
#endif
#ifdef FLECS_TIMER
#ifdef FLECS_NO_TIMER
#error "FLECS_NO_TIMER failed: TIMER is required by other addons"
#endif
#include "../addons/timer.h"
#endif
#ifdef FLECS_PIPELINE
#ifdef FLECS_NO_PIPELINE
#error "FLECS_NO_PIPELINE failed: PIPELINE is required by other addons"
#endif
#include "../addons/pipeline.h"
#endif
#ifdef FLECS_SYSTEM
#ifdef FLECS_NO_SYSTEM
#error "FLECS_NO_SYSTEM failed: SYSTEM is required by other addons"
#endif
#include "../addons/system.h"
#endif
#ifdef FLECS_STATS
#ifdef FLECS_NO_STATS
#error "FLECS_NO_STATS failed: STATS is required by other addons"
#endif
#include "../addons/stats.h"
#endif
#ifdef FLECS_MONITOR
#ifdef FLECS_NO_MONITOR
#error "FLECS_NO_MONITOR failed: MONITOR is required by other addons"
#endif
#include "../addons/monitor.h"
#endif
#ifdef FLECS_COREDOC
#ifdef FLECS_NO_COREDOC
#error "FLECS_NO_COREDOC failed: COREDOC is required by other addons"
#endif
#include "../addons/coredoc.h"
#endif
#ifdef FLECS_DOC
#ifdef FLECS_NO_DOC
#error "FLECS_NO_DOC failed: DOC is required by other addons"
#endif
#include "../addons/doc.h"
#endif
#ifdef FLECS_JSON
#ifdef FLECS_NO_JSON
#error "FLECS_NO_JSON failed: JSON is required by other addons"
#endif
#include "../addons/json.h"
#endif
#if defined(FLECS_EXPR) || defined(FLECS_META_C)
#ifndef FLECS_META
#define FLECS_META
#endif
#endif
#ifdef FLECS_UNITS
#ifdef FLECS_NO_UNITS
#error "FLECS_NO_UNITS failed: UNITS is required by other addons"
#endif
#include "../addons/units.h"
#endif
#ifdef FLECS_META
#ifdef FLECS_NO_META
#error "FLECS_NO_META failed: META is required by other addons"
#endif
#include "../addons/meta.h"
#endif
#ifdef FLECS_EXPR
#ifdef FLECS_NO_EXPR
#error "FLECS_NO_EXPR failed: EXPR is required by other addons"
#endif
#include "../addons/expr.h"
#endif
#ifdef FLECS_META_C
#ifdef FLECS_NO_META_C
#error "FLECS_NO_META_C failed: META_C is required by other addons"
#endif
#include "../addons/meta_c.h"
#endif
#ifdef FLECS_PLECS
#ifdef FLECS_NO_PLECS
#error "FLECS_NO_PLECS failed: PLECS is required by other addons"
#endif
#include "../addons/plecs.h"
#endif
#ifdef FLECS_RULES
#ifdef FLECS_NO_RULES
#error "FLECS_NO_RULES failed: RULES is required by other addons"
#endif
#include "../addons/rules.h"
#endif
#ifdef FLECS_SNAPSHOT
#ifdef FLECS_NO_SNAPSHOT
#error "FLECS_NO_SNAPSHOT failed: SNAPSHOT is required by other addons"
#endif
#include "../addons/snapshot.h"
#endif
#ifdef FLECS_PARSER
#ifdef FLECS_NO_PARSER
#error "FLECS_NO_PARSER failed: PARSER is required by other addons"
#endif
#include "../addons/parser.h"
#endif
#ifdef FLECS_HTTP
#ifdef FLECS_NO_HTTP
#error "FLECS_NO_HTTP failed: HTTP is required by other addons"
#endif
#include "../addons/http.h"
#endif
#ifdef FLECS_OS_API_IMPL
#ifdef FLECS_NO_OS_API_IMPL
#error "FLECS_NO_OS_API_IMPL failed: OS_API_IMPL is required by other addons"
#endif
#include "../addons/os_api_impl.h"
#endif
#ifdef FLECS_MODULE
#ifdef FLECS_NO_MODULE
#error "FLECS_NO_MODULE failed: MODULE is required by other addons"
#endif
#include "../addons/module.h"
#endif

#ifdef FLECS_CPP
#ifdef FLECS_NO_CPP
#error "FLECS_NO_CPP failed: CPP is required by other addons"
#endif
#include "../addons/flecs_cpp.h"

#ifdef __cplusplus
}

#include "../addons/cpp/flecs.hpp"

extern "C" {
#endif // __cplusplus

#endif // FLECS_CPP

#endif
