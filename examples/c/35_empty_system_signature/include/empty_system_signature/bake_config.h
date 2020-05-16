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

#ifndef EMPTY_SYSTEM_SIGNATURE_BAKE_CONFIG_H
#define EMPTY_SYSTEM_SIGNATURE_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <flecs.h>

/* Headers of private dependencies */
#ifdef EMPTY_SYSTEM_SIGNATURE_IMPL
/* No dependencies */
#endif

/* Convenience macro for exporting symbols */
#ifndef EMPTY_SYSTEM_SIGNATURE_STATIC
  #if EMPTY_SYSTEM_SIGNATURE_IMPL && (defined(_MSC_VER) || defined(__MINGW32__))
    #define EMPTY_SYSTEM_SIGNATURE_EXPORT __declspec(dllexport)
  #elif EMPTY_SYSTEM_SIGNATURE_IMPL
    #define EMPTY_SYSTEM_SIGNATURE_EXPORT __attribute__((__visibility__("default")))
  #elif defined _MSC_VER
    #define EMPTY_SYSTEM_SIGNATURE_EXPORT __declspec(dllimport)
  #else
    #define EMPTY_SYSTEM_SIGNATURE_EXPORT
  #endif
#else
  #define EMPTY_SYSTEM_SIGNATURE_EXPORT
#endif

#endif

