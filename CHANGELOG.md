flecs
CHANGELOG
=======================================================================

RELEASE NOTES:                  https://github.com/SanderMertens/flecs/releases
REPO:                           https://github.com/SanderMertens/flecs/commits/master
COMMUNITY:                      https://discord.gg/MRSAZqb
DOCS:                           https://flecs.docsforge.com/
FAQ:                            https://github.com/SanderMertens/flecs/blob/master/docs/FAQ.md
QUICKSTART:                     https://github.com/SanderMertens/flecs/blob/master/docs/Quickstart.md
MANUAL:                         https://github.com/SanderMertens/flecs/blob/master/docs/Manual.md

-----------------------------------------------------------------------
 VERSION 2.3.2 (Released 2021-02-23)
-----------------------------------------------------------------------

## Improvements
- replace iostream with initializer_list (C++ API, thanks @ikrima!)
- ensure entity::m_id is initialized to 0 (C++ API, thanks @ikrima!)
- use ecs_os_malloc instead of new (C++ API, thanks @ikrima!)
- remove superfluous copies of lambda functions (C++ API, thanks @ikrima!)
- add CHANGELOG (thanks @ikrima!)

## Bugfixes
- fix matching for queries with shared componnents when base entity is deleted
- fix various issues with component registration in C++
- fix issue with setting target FPS on Apple M1 (thanks @prime31!)
- fix issues with CMake file (thanks @Spacelm!)
- fix crash when creating & deleting queries
- guarantee that id returned by new_component_id is unused


-----------------------------------------------------------------------
 VERSION 2.3.1 (Released 2021-02-02)
-----------------------------------------------------------------------

## Improvements
- Improved lerp example
- Added OS API example C++ (thanks @mcmlevi!)
- Upgraded cmake buildfiles (thanks @rawbby!)
- Clarified text in README describing API/ABI stability

## Bugfixes
- Fix crash when using overlapping UnSet systems
- Fix issue with passing incorrect row to UnSet systems
- Added .hpp files to cmake install
- Fixed issue with using get_mut with traits


-----------------------------------------------------------------------
 VERSION 2.3.0 (Released 2021-01-17)
-----------------------------------------------------------------------

## Highlights
- Big performance improvements & reduced memory usage for applications with lots of tables, such as when using hierarchies
- Component enabling/disabling allows for quick component mutations without moving entities between archetypes
- New statistics addon for retrieving metrics from running world (replaces stats module)

Thanks to @randy408, @sh-dave, @kevinresol, @jasonliang-dev and @Alexandre-P-J for submitting PRs! 🎉

Thanks to @ikrima and @jtferson for writing two awesome testimonials on using Flecs with Unreal Engine 4 🙏 :
- https://bebylon.dev/blog/ecs-flecs-ue4/
- https://jtferson.github.io/blog/flecs_and_unreal/

Thanks to the new Flecs sponsors ❤️ :
  - @Zifkan
  - @TFlippy
  - @Hexlord

## Breaking changes
- calling ecs_get for a tag will no longer return NULL, but will assert
- statistics module is removed in favor of easier to use statistics addon
- unused table_offset member is removed from iterator

## Deprecated features
- ecs_entity() macro is now deprecated, use ecs_id()

## Features
- Direct access addon, which provides fast access to underlying storage
- Added singleton API to C++ world class
- Added orphaning for subqueries, which allows an application to check if a parent query still exists
- Added ecs_get_typeid function to get the component id for traits
- The type used for time keeping is now customizable
- New statistics addon for retrieving metrics from running world
- Added get_parent function that accepts entity/tag
- Added component enabling/disabling
- Added support for sorting on shared components

## Improvements
- Improved ecs_delete performance (reduced entity index accesses from 2 to 1)
- C & C++ code compiles warning-free with more warning flags, for more compiler versions & more platforms
- Improved error messages when application passes invalid entity id
- Made include paths relative so source code is easier to integrate with bazel
- Fixed typos in documentation
- Improve error message when trying to add 0 as base entity (using ECS_INSTANCEOF)
- Add check for conflicting source modifier in OR expressions
- Extended documentation, added new examples and fixed typos
- Removed dead/redundant code
- Improved performance of ecs_get
- Add sanitizer & custom builds to CI, remove Travis CI builds
- Don't add Name component when provided name for entity is NULL
- Allow flecs::system instances to be (re)created from entity id
- Added godbolt "try online" badge to README
- Improve allocation strategy of vector datastructure
- Improved performance for checking if entity id is a number
- Added missing query functions to C++ API for sorting
- Improved performance of table creation by two orders of magnitude
- Reduced memory footprint of table graph by a factor 60
- Added example to demonstrate world merging with direct access API
- Added more inline documentation for datastructures
- Added assert when trying to instantiate child as instance
- Improve errors when invalid operation is invoked on iterator
- Throw assert when invoking ecs_modified on entity that does not have the component
- Allow for type lookups in systems, as long as the type already exists
- Add return types to ecs_defer_begin and ecs_defer_end to obtain defer status of world
- Remove redundant table_offset member from ecs_iter_t
- Improved portability of POSIX OS API example

## Bugfixes
- Fixed issues with subqueries and query rematching
- Corrected wrong return type of ecs_term_size (was ecs_entity_t, is now size_t)
- Add missing \0 when appending to application buffer with strbuf datastructure
- Fixed crash when instantiating an empty prefab
- Fixed issue with shared tags and queries using each() in the C++ API
- Fixed issue with instantiating empty child table
- Fixed issue with ecs_bulk_new and traits
- Fixed issue when using ecs_id_str with small buffers
- Fixed bug when adding trait to entity with switch
- Fixed name conflicts in amalgamated source
- Fixed path lookup in ecs_import
- Fixed EXPORT macro's in OS API examples
- Fixed issue with restoring worlds that have recycled ids
- Added missing EXPORT macro's to API functions
- Fixed assert after deleting entities from restored world
- Removed obsolete assert from ecs_modified
- Added missing statement to finalize system in C++
- Fixed issues with removing case values
- Fixed issues in C++ API with implicit component registration
- Fixed assert when removing from switch list
- Fixed bug where obtaining type from entity would generate a new entity id
- Fixed incorrect description of ecs_delete in manual
- Fixed issues with custom builds
