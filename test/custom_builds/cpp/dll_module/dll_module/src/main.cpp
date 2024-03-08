#include <dll_module.h>

namespace movement 
{
   module::module(flecs::world& ecs) {
      ecs.module<module>();

      ecs.component<Position>();
      ecs.component<Velocity>();
   }
}
