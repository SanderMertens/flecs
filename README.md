# Unreal Engine Flecs Integration

### Unreal-Flecs Discord: https://discord.gg/Q6EWgd8tFK

### Flecs Discord: https://discord.gg/WRn8QSfVvx

### Note:

Many Features are not complete

Only tested on Version: UE 5.7 MSVC/partially Clang

### Contributors to Unreal-Flecs:

@Megafunk \
@ronaldburns 


### For Full Performance benefits make sure to use LTO/PGO in shipping builds.

## Wiki/Documentation coming soon

## Features:
Universalized Component Type Registration CPP <--> Meta Type, Works with USTRUCT, UENUM, and UCLASS \
\
OS-API implementation for Unreal \
\
Tracked ObjectPtr References in Components (needs testing) \
\
Module Implementation with Hard and Soft Dependencies \
\
Modular Game Loop Implementation to allow for any pipeline setup (also works with module dependencies) \
\
Partial Natvis support for different flecs types


## Modifications to Flecs:

New Component Registration in `Source/FlecsLibrary/Public/flecs/addons/cpp/component.hpp`
Disabled flecs Auto Component Registration (do not re-enable)
set\_symbol in `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/entity/builder.hpp`

to\_constant in `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/impl.hpp` and `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/entity\_view.inl`

enum\_data<E>::entity(underlying\_type\_t<E> value) in `Source/FlecsLibrary/Public/flecs/addons/cpp/impl/world.hpp`

EcsFirstUserComponentId is set to 9

added bUseLowId bool paramn to untyped_component(flecs::world_t *world, const char *name) constructor in `Source/FlecsLibrary/Public/flecs/addons/cpp/component.hpp`

`ensure` in the Entity CPP-API is renamed to `obtain` to be compatible with Unreal
