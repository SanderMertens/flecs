# Unreal Engine Flecs Integration

## Depends on [Solid-Macros](https://github.com/Reddy-dev/Solid-Macros)

### Unreal-Flecs Discord: https://discord.gg/Q6EWgd8tFK

### Flecs Discord: https://discord.gg/WRn8QSfVvx

### Note:

Many Features are not complete

Only tested on Version: UE 5.7 MSVC/partially Clang

### Contributors to Unreal-Flecs:

@Megafunk \
@ronaldburns 

## Features:
Universalized Component Type Registration CPP <--> Meta Type, Works with USTRUCT, UENUM, and UCLASS \
Tracked ObjectPtr References in Components

## Modifications to Flecs:

New Component Registration in `Source/FlecsLibrary/Public/flecs/addons/cpp/component.hpp`
Disabled flecs Auto Component Registration (do not re-enable)
set\_symbol in `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/entity/builder.hpp`

to\_constant in `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/impl.hpp` and `Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/entity\_view.inl`

enum\_data<E>::entity(underlying\_type\_t<E> value) in `Source/FlecsLibrary/Public/flecs/addons/cpp/impl/world.hpp`

EcsFirstUserComponentId is set to 9

added bUseLowId bool paramn to untyped_component(flecs::world_t *world, const char *name) constructor in `Source/FlecsLibrary/Public/flecs/addons/cpp/component.hpp`

`ensure` in the Entity CPP-API is renamed to `obtain` to be compatible with Unreal
