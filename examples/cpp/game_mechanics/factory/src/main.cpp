#include <factory.h>
#include <iostream>

// This example shows how a simple production hierarchy can be simulated in ECS.
// The example has the following concepts:
//
// - A recipe describes the requirements and time required to produce a resource
// - Factories produce resources based on an assigned recipe
// - Depots store resources
//
// A factory needs to be connected to inputs that provide the resource. Once the
// inputs are connected correctly, the factory will start producing the resource
// once it has collected all of the required resources from the inputs.
//
// Factories can act as inputs for other factories. Depots can be used to 
// combine the output of multiple factories.
//
// The resource definitions and scene are loaded at runtime from resource.flecs
// and scene.flecs.
//
// Few notes:
// - The demo doesn't have graphics, but can be viewed with the explorer.
// - Components store references to entities as entity_t's. They could also
//   flecs::entity's, but this requires a bit less memory.
// - Some data is duplicated between resource and factory components so that
//   systems can mostly operate on local data
//

using namespace flecs;

// Factory module
struct factories {
  // Maximum number of resources a recipe can depend on
  static constexpr int MaxInputs = 3;


  // -- Recipe components

  // Single resource requirement for a recipe
  struct Requirement {
    entity_t resource;
    int32_t amount;
  };

  // All resource requirements for a recipe
  struct Requires {
    Requirement items[MaxInputs];
  };

  // Time it takes to produce a resource
  struct TimeToProduce {
    float value;
  };

  // Limit how many items can be stacked for a specific resource
  struct StackCount {
    int32_t amount;
  };


  // -- Depot & Factory components

  // Resource storage
  struct Stores {
    int32_t amount;
  };

  // Factory configuration
  struct Factory {
    // Recipe for the resource to produce
    entity_t recipe;

    // Must provide resources as specified by recipe
    entity_t inputs[MaxInputs];

    // Optional output which allows a factory to forward items to a depot
    entity_t output;
  };

  // Factory state
  enum class FactoryState {
    // Factory isn't connected to (correct) inputs yet
    Idle,

    // Factory is connected, but doesn't have the required resources yet
    WaitingForResources,

    // Factory is producing resource
    Producing,

    // Factory is done producing
    TransferResource
  };

  // Tracks which resources have been supplied to factory
  struct FactorySupply {
    int32_t required[MaxInputs];
    int32_t collected[MaxInputs];
    flecs::ref<Stores> inputs[MaxInputs];
    flecs::ref<Stores> output;
  };

  // Track production progress
  struct FactoryProduction {
    float value;
    float duration;
    int32_t max_stack;
  };

  // Module import function
  factories(world& world) {
    // Units improve visualization of component values in the explorer.
    world.import<flecs::units>();

    // -- Component registration
    // Reflection data is registered so we can view the components from the
    // explorer and assign them from .flecs files.

    world.component<Stores>()
      .member<int32_t>("amount");

    world.component<Requirement>()
      .member(flecs::Entity, "resource")
      .member<int32_t>("amount");

    world.component<Requires>()
      .array<Requirement>(3);

    world.component<FactorySupply>()
      .member<int32_t>("required", 3)
      .member<int32_t>("collected", 3);

    world.component<TimeToProduce>()
      .member<float, units::duration::Seconds>("value");

    world.component<StackCount>()
      .member<int32_t>("amount");

    world.component<FactoryProduction>()
      .member<float, units::Percentage>("value")
      .member<float, units::duration::Seconds>("duration")
      .member<int32_t>("max_stack");

    world.component<Factory>()
      // Factories start out idle
      .on_add([](entity factory, Factory) {
        factory.add(FactoryState::Idle);
      })
      // Check if factory is properly setup when value is assigned to component
      .on_set(factory_init)
      .member(flecs::Entity, "recipe")
      .member(flecs::Entity, "inputs", 3)
      .member(flecs::Entity, "output");

    // System that collects resources from inputs
    world.system<FactorySupply>("Collect")
      .with(FactoryState::WaitingForResources)
      .interval(1.0f)
      .each([](entity factory, FactorySupply& s) {
        bool satisfied = true;

        for (int i = 0; i < MaxInputs; i ++) {
          int32_t needed = s.required[i] - s.collected[i];
          if (needed) {
            Stores *p = s.inputs[i].get();
            if (p->amount >= needed) {
              s.collected[i] += needed;
              p->amount -= needed;
            } else {
              s.collected[i] += p->amount;
              p->amount = 0;
              satisfied = false;
            }
          }
        }

        // If all resources are satisfied, change state of factory to producing
        if (satisfied) {
          // Consume collected resources
          for (int i = 0; i < MaxInputs; i ++) {
            s.collected[i] = 0;
          }

          factory.add(FactoryState::Producing);
        }
      });

    // System that produces a resource once all input requirements are met
    world.system<FactoryProduction>("Produce")
      .with(FactoryState::Producing)
      .interval(0.1f)
      .each([](flecs::iter& it, size_t i, FactoryProduction& p) {
        p.value += it.delta_system_time() / p.duration;
        if (p.value >= 1) {
          p.value = 1;
          it.entity(i).add(FactoryState::TransferResource);
        }
      });

    // System that transfers resource and resets factory for next item
    world.system<FactorySupply, FactoryProduction, Stores>("Transfer")
      .term_at(2).second(Wildcard)
      .with(FactoryState::TransferResource)
      .interval(1.0f)
      .each([](entity factory, FactorySupply& s, FactoryProduction& p, Stores& out) {
        // Reset production progress
        p.value = 0;

        // If depot is configured, transfer local resources first
        Stores *depot = s.output.try_get();
        if (depot) {
          int32_t available_space = p.max_stack - depot->amount;
          if (out.amount > available_space) {
            depot->amount += available_space;
            out.amount -= available_space;
          } else {
            depot->amount += out.amount;
            out.amount = 0;
          }
        }

        // Now check which storage to output the new resource item to. If a 
        // depot is configured, attempt to store it there, otherwise store it
        // in factory.
        Stores *store = &out;
        if (depot) {
          if (!p.max_stack || (depot->amount < p.max_stack)) {
            store = depot;
          }
        }

        if (store->amount < p.max_stack) {
          // Add resource to storage and go back to collecting resources
          store->amount ++;
          factory.add(FactoryState::WaitingForResources);
        } else {
          // No space in output, do nothing
        }
      });
  }

private:
  // on_set hook for Factory component. When Factory component value is written, 
  // check if the factory has a recipe and whether the factory inputs satisfy 
  // the recipe requirements.
  static void factory_init(entity factory, Factory& config) {
    world world = factory.world();
    entity recipe = world.entity(config.recipe);

    recipe.get([&](const Requires& r) {
      entity output = world.entity(config.output);
      if (output) {
        if (!output.has<Stores>(recipe)) {
          std::cout << factory.path() << ": output doesn't provide resource " 
            << recipe.path() << "\n";
          return;
        }
      }

      // For each recipe requirement, make sure a correct input is connected
      bool satisfied = true;
      for (int i = 0; i < MaxInputs; i ++) {
        entity resource = world.entity(r.items[i].resource);
        entity input = world.entity(config.inputs[i]);
        if (!resource) {
          if (input) {
            std::cout << factory.path() << 
              ": input connected to empty input slot\n";
          }
          continue;
        }

        if (resource && !input) {
          satisfied = false;
          break;
        }

        if (!input.has<Stores>(resource)) {
          std::cout << factory.path() << ": input doesn't provide resource " 
            << recipe.path() << "\n";
          satisfied = false;
          break;
        }
      }

      // If we got through all requirements without issues, factory is ready
      // to start collecting resources
      if (satisfied) {
        factory.add(FactoryState::WaitingForResources);

        // Initialize supply component
        factory.insert([&](FactorySupply& s) {
          for (int i = 0; i < MaxInputs; i ++) {
            entity resource = world.entity(r.items[i].resource);
            entity input = world.entity(config.inputs[i]);
            int32_t amount = r.items[i].amount;

            s.required[i] = amount;
            s.collected[i] = 0;

            if (!resource) {
              continue;
            }

            // Take a ref to the resource provider for the factory input. Refs
            // provide quick and convenient access for repeatedly accessing a
            // component from the same entity.
            s.inputs[i] = input.get_ref<Stores>(resource);
          }

          if (output) {
            // If the factory has an output configured, also take a ref to it so
            // we can quickly add the produced resources to it.
            s.output = output.get_ref<Stores>(recipe);
          }
        });

        // Add component that tracks how much time is left to produce a resource
        recipe.get([&](const TimeToProduce& ttp, const StackCount& sc) {
          factory.set<FactoryProduction>({ 0, ttp.value, sc.amount });
        });

        // Set output resource for factory
        factory.add<Stores>(recipe);
      }
    });
  }
};

int main(int argc, char *argv[]) {
  flecs::world world(argc, argv);

  world.import<factories>();

  ecs_script_run_file(world, "resources.flecs");
  ecs_script_run_file(world, "scene.flecs");

  return world.app()
    .enable_rest()
    .target_fps(60)
    .delta_time(1.0f / 60.0f) // Run at fixed time step
    .run();
}
