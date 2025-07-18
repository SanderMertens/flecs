# Unreal Engine Flecs Integration

## Depends on [Solid-Macros](https://github.com/Reddy-dev/Solid-Macros)

### Unreal-Flecs Discord: https://discord.gg/Q6EWgd8tFK

### Flecs Discord: https://discord.gg/WRn8QSfVvx

### Note:

Many Features are not complete

Only tested on Version: UE 5.6 MSVC

### Contributors to Unreal-Flecs:

@Megafunk

### Modifications to Flecs:

New Component Registration in Source/FlecsLibrary/Public/flecs/addons/cpp/component.hpp  
Disabled flecs Auto Component Registration  
set\_symbol in Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/entity/builder.hpp

to\_constant in Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/impl.hpp and Source/FlecsLibrary/Public/flecs/addons/cpp/mixins/enum/entity\_view.inl

enum\_data<E>::entity(underlying\_type\_t<E> value) in Source/FlecsLibrary/Public/flecs/addons/cpp/impl/world.hpp

