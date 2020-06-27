/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef COLLECTIONS_BAKE_CONFIG_H
#define COLLECTIONS_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <flecs.h>
#ifdef __BAKE__
#include <bake_util.h>
#endif
#include <bake_test.h>

/* Headers of private dependencies */
#ifdef collections_EXPORT
/* No dependencies */
#endif

/* Convenience macro for exporting symbols */
#if collections_EXPORTS && (defined(_MSC_VER) || defined(__MINGW32__))
  #define COLLECTIONS_EXPORT __declspec(dllexport)
#elif collections_EXPORTS
  #define COLLECTIONS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
  #define COLLECTIONS_EXPORTS __declspec(dllimport)
#else
  #define COLLECTIONS_EXPORT
#endif

#endif

