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

#ifndef REPLACE_BAKE_CONFIG_H
#define REPLACE_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <flecs.h>

/* Headers of private dependencies */
#ifdef REPLACE_IMPL
/* No dependencies */
#endif

/* Convenience macro for exporting symbols */
#ifndef REPLACE_STATIC
  #if REPLACE_IMPL && (defined(_MSC_VER) || defined(__MINGW32__))
    #define REPLACE_EXPORT __declspec(dllexport)
  #elif REPLACE_IMPL
    #define REPLACE_EXPORT __attribute__((__visibility__("default")))
  #elif defined _MSC_VER
    #define REPLACE_EXPORT __declspec(dllimport)
  #else
    #define REPLACE_EXPORT
  #endif
#else
  #define REPLACE_EXPORT
#endif

#endif

