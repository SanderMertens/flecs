# Designing with Flecs
Designing games with ECS can be overwhelming and quite different from object oriented approaches, not to mention learning all the framework specific features! This guide provides a few quick tips for using the features provided by Flecs to build fast, readable and reusable code.

Note that these are my own guidelines, and as a result this is an opinionated document. There are many other approaches to designing with ECS, and it would be silly to claim that this is "the one true way". Take this with a grain of salt, and feel free to deviate when you feel it is appropriate.

One other note: this document is very light on feature documentation. The point of the document is to provide suggestions for how to design with Flecs features, not document their ins and outs. All of the features listed in this document are described in the manual and have example code.

## Entities
Entities are probably the easiest thing to get used to. They typically map to in-game objects the same way you would create them in other game engines.

### Entity Initialization
When creating entities, you typically want to initialize them with a set of default components, and maybe even default values. Flecs introduced prefabs for this use case. Prefabs let you create entity templates that contain components with default values. Creating entities from prefabs is not just an easy way to initialize entities, it is also faster, and helps with classifying the different kinds of entities you have.

### Entity Lifecycle
Entities can be created and deleted dynamically. When entities are deleted, the existing handles to that entity are no longer valid. When you are working with entity handles, it is often good practice to make sure they are still alive. You can do this with the `is_alive()` function. Other than that, entity handles are stable, which means that you can safely store them in your components or elsewhere.

### Entity Names
Flecs entities can be named. This makes it easy to identify entities in editors or while debugging, and also allows you to lookup entities by name. Names must be unique inside a scope, which is determined by the `ChildOf` relationship. For example, two entities with the same parent must have different names. 

Names can be looked up using a relative path or absolute path using the `ecs_lookup_fullpath` or `entity::lookup` functions. The default scope separator for C applications is a dot (`.`), whereas in C++ it is a double colon (`::`). Lookups use a hashmap internally, which provides O(1) performance.

Entities can be assigned user friendly names with the doc addon, using the `ecs_doc_set_name` or `entity::set_doc_name` functions. Names assigned by the doc framework do not have to be unique.

## Components
Designing your components is probably the most important thing you will do in your ECS application. The reason is that if you change a component you have to update all systems that use it. Fortunately components can be designed in a way that minimizes refactoring and does not negatively impact performance.

### Component Size
The first tip is to try keep your components small and atomic. If you are choosing between a Transform component or separate Position, Rotation, Scale, Matrix components pick the latter. If you have a Turret component with a target and a rotation angle, split them up into two components.

The first reason behind this is that querying for multiple components adds minimal overhead, because most queries are cached.

The second reason is that it improves caching performance. If your system only needs Position, but also has to load all of the other data in Transform, you end up loading a lot of data in your cache that is not used. This means that useful data will get evicted from the cache more often, and that data needs to be loaded from RAM more often, which is much slower than when data is cached.

The third reason is that it minimizes refactoring. There are only so many ways to split up components, and infinitely more ways to combine them. Once components are designed as atomic units of data there aren't many reasons to combine them which results in less refactoring.

The fourth and last reason is that code using smaller components is more reusable. Reasons to combine two components may work well in one project, but not in another project. Atomic components are less opinionated since it's up to the project how they are combined in queries, which makes it more likely that they work well across projects. This trickles down to systems, as systems written for atomic components also end up being more reusable.

A disadvantage of small components is that you get more of them in a project. This can make it harder to find the components a system needs, especially in large projects with hundreds of components. Tools like https://www.flecs.dev/explorer/ can help with finding and documenting components in a project.

### Complex component data
There is a misconception that ECS components can only be plain data types, and should not have vectors, or more complex data structures. The reality is  more nuanced. You may find yourself often needing specialized data structures, and it is perfectly fine to store these in components.

## Queries
Queries are the primary method in Flecs for finding the entities for a set of components (or more specifically: a component expression). Queries are easy to use, but there a few things to keep in mind.

### Use the right query
Flecs has cached queries and uncached queries. Cached queries (`ecs_query_t` and `flecs::query`) are expensive to create but very cheap to iterate. Uncached queries (`ecs_filter_t`, `flecs::filter`) are fast to create, but more expensive to iterate. If you need to do a quick ad-hoc query for which you couldn't know in advance what you had to query for, an uncached query is the best option. If you have a query that you know in advance and need to iterate many times, a cached query is preferred.

Another difference is that uncached queries can be created from systems, while cached queries cannot. If you need a cached query in a system, it has to be created in advance and passed into the system, either by setting it as system context, adding a component to the system with the query, or passing it in the lambda capture list (C++ only). Systems themselves use cached queries.

Make sure to not repeatedly create and destroy cached queries! For more information, see [the query manual](https://www.flecs.dev/flecs/#/docs/Queries?id=query-types) for more details.

### Use in/inout/out annotations
Flecs analyzes how components are read and written by queries and uses this for things like change tracking and deciding when to merge command buffers. By default components are marked as `inout`. If a system only reads a component, make sure to mark it as `in`, as this can reduce the time spent by other parts of the application that rely on change detection and sync points.

For more information, see [the query manual](https://www.flecs.dev/flecs/#/docs/Queries?id=access-modifiers).

### Annotations
You can further annotate queries with components that are not matched with the query, but that are written using ECS operations (like add, remove, set etc.). Such operations are automatically deferred and merged at the end of the frame. With annotations you can enforce merging at an earlier point, by specifying that a component modification has been queued. When Flecs sees this, it will merge back the modifications before the next read.

See the sync point examples for more detail:
C: https://github.com/SanderMertens/flecs/blob/master/examples/c/systems/sync_point
C++: https://github.com/SanderMertens/flecs/blob/master/examples/cpp/systems/sync_point

## Systems
Designing systems is one of the hardest things to do when not coming from an ECS background. Object oriented code allows you to write logic that is local to a single object, whereas systems in ECS are ran for collections of similar objects. This requires a different approach towards design.

### System Scope
Try to design your systems with a single responsibility. This can sometimes be difficult, especially if you are building new features and are not exactly sure yet what the end result will look like. That is fine. It is better to start with something that works, and refine it afterwards. If you find yourself with a system that does a lot of things, don't worry. Because systems in ECS are decoupled from everything else, it is generally pretty easy to split them up.

While it is perfectly fine to have large systems in an ECS application, there are a few advantages to keeping them small:

- Smaller systems make it easier to isolate behavior, as you can simply remove systems from your application that you don't want to test.
- Smaller systems usually have less complicated code, which can allow for more compiler optimizations (like auto vectorization).
- Smaller systems are easier to reuse across project.

### System Scheduling
Flecs has the ability to run and schedule your systems for you. The advantage of this, versus manually listing and running systems, is that it is easier to import systems from multiple, reusable modules.

A system is basically a combination of three things: a query, a function, and ordering information. The query finds the right entities, and the function is invoked with the matched entities. The ordering information makes sure that the system is inserted in the right point in your frame. If you get this right, you can import any number of systems into your project, and you can just run them without spending any time on manually sorting them out.

The ordering information consists out of a phase (see phases and pipelines) and an implicit declaration order. Systems are ordered according to their phases first. Within a phase, they are ordered by declaration order. This may feel rigid, but is very deliberate. It prevents you from defining dependencies between systems, which can make it difficult to reuse systems across projects.

On the other hand, if you are working with an existing framework or engine, you may not have the luxury of scheduling everything yourself. The engine may for example provide you with callbacks in which you need to do certain logic. Maybe you want to build your own threading system. In those situations it can make sense to take control of running systems yourself. 

Sometimes you may even not use systems at all, and just run queries. In this case you may want to disable the system addon (see the addsons section in the README). Note that some Flecs features depend on systems, like the REST API and timers.

## Phases and Pipelines
Phases and pipelines are the primitives that Flecs uses to order systems. A pipeline is a set of ordered phases. Systems can be assigned to those phases. When using phases and pipelines correctly, it allows you to write plug & play systems that are easy to reuse in different projects.

### Selecting a Phase
When you create a system, you can assign a phase to it. By default, that phase is `OnUpdate`. Flecs comes with a whole bunch of phases though, and just looking at the whole list can feel a bit overwhelming:

- `OnLoad`
- `PostLoad`
- `PreUpdate`
- `OnUpdate`
- `OnValidate`
- `PostUpdate`
- `PreStore`
- `OnStore`

So what do these all mean? Actually they mean nothing at all! They are just tags you can assign to systems, and those tags ensure that all systems in, say, the `PreUpdate` phase are executed _before_ the systems in the `OnUpdate` phase. What is also important to realize is that this list of phases is only the default provided by Flecs. Maybe your project needs only half of those, or maybe it needs entirely different ones! Flecs lets you specify your custom phases to match your project needs.

There are some conventions around the builtin phases, and following them helps to ensure that your code works well with the Flecs module ecosystem. Here they are:

### OnLoad
This phase contains all the systems that load data into your ECS. This would be a good place to load keyboard and mouse inputs.

### PostLoad
Often the imported data needs to be processed. Maybe you want to associate your keypresses with high level actions rather than comparing explicitly in your game code if the user pressed the 'K' key. The PostLoad phase is a good place for this.

### PreUpdate
Now that the input is loaded and processed, it's time to get ready to start processing our game logic. Anything that needs to happen after input processing but before processing the game logic can happen here. This can be a good place to prepare the frame, maybe clean up some things from the previous frame, etcetera.

### OnUpdate
This is usually where the magic happens! This is where you put all of your gameplay systems. By default systems are added to this phase.

### OnValidate
This phase was introduced to deal with validating the state of the game after processing the gameplay systems. Sometimes you moved entities too close to each other, or the speed of an entity is increased too much. This phase is for righting that wrong. A typical feature to implement in this phase would be collision detection.

### PostUpdate
When your game logic has been updated, and your validation pass has ran, you may want to apply some corrections. For example, if your collision detection system detected collisions in the OnValidate phase, you may want to move the entities so that they no longer overlap.

### PreStore
Now that all of the frame data is computed, validated and corrected for, it is time to prepare the frame for rendering. Any systems that need to run before rendering, but after processing the game logic should go here. A good example would be a system that calculates transform matrices from a scene graph.

### OnStore
This is where it all comes together. Your frame is ready to be rendered, and that is exactly what you would do in this phase.

That was a quick overview of all the builtin phases. Note that these are just guidelines! Feel free to deviate if your project calls for it.

### Custom phases and pipelines
An application can add phases to the existing list, or define a pipeline from scratch. See the following examples on how to do this:

C:
https://github.com/SanderMertens/flecs/tree/master/examples/c/systems/custom_phases
https://github.com/SanderMertens/flecs/tree/master/examples/c/systems/custom_pipeline

C++
https://github.com/SanderMertens/flecs/tree/master/examples/cpp/systems/custom_phases
https://github.com/SanderMertens/flecs/tree/master/examples/cpp/systems/custom_pipeline

## Modules
Large applications can often contain many components and systems. Some large commercial projects have reported up to 800 components! Managing all those components and systems becomes important on that scale, and that is what modules are for. Modules are one of those features that you usually don't think about when selecting an ECS, but they can make your life a lot easier.

### Defining Modules
The purpose of modules is really to enable reusability. A well written module can be imported into any project, and will do its thing without any tweaking or tinkering. To achieve this, make sure to define your modules around features. Features seldomly consist out of a single system or component, but can have many. Examples are rendering, collision detection, input management, and so on.

### Module Dependencies and Ordering
Modules can depend on each other. In fact, often do! Importing a module twice has no penalties in Flecs, it will not define your systems and components twice. This enables your application code to import the modules it needs, without having to worry about whether they were already loaded.

The order in which you import dependencies in a module is important. If you define systems before the import, those systems will be ran _before_ the systems from the imported module _within the same phase_ (read this line a few times until you understand, it's important). This allows you some degree of flexibility around how systems from different modules should be scheduled.

What is key to note here is that the granularity of control is at the module level, _never_ at the individual system level. The reason for this is that modules may reimplement their features with different systems. If you have inter-system dependencies, those could break easily every time you update a module. This also makes sure that you can replace one module for another without running into annoying compatibility issues.

### Modules and Feature Swapping
A good practice to employ with modules is to split them up into components.* modules and systems.* modules. For example, you may have a module `components.physics` that contains all the components to store data for a physics system. You may then have a module called `systems.physics`, which imports the components module and "implements" the components. Because applications only have access to `components.physics` (they import it as well) but do not have direct access to the systems inside `systems.physics`, you can simply swap one physics implementation with another without changing application code.

This is a powerful pattern enabled by ECS that will give your projects a lot of flexibility and freedom in refactoring code.

### Module Overhead
You might wonder whether a module with lots of systems, of which only a few are used by your application makes your application slower. The answer is fortunately no. Flecs only inserts systems into the main loop that have matched with actual entities. Any imported systems that have never matched with anything remain dormant, and will not negatively affect performance.

## Relationships
When you are working with Flecs, chances are that at some point you'll want to use relationships. The two most common uses for relationships are hierarchies like a scene graph (the `ChildOf` relationship) and prefabs (the `IsA` relationship).

In many cases you might want to use your own relationships. Here are a few signs to look out for that can tell you to think about relationships:
- You have a component with an entity handle, and you need to find all entities that point to a specific entity.
- You have many components with the same or similar set of members, and systems that duplicate code for each component.
- You need to store multiple instances of the same component on an entity.
- You're designing some kind of container structure, like an inventory.
- You are looking to group entities by something like world cells or layers, and want to be able to lookup all entities for a cell.
- You're adding an enumeration type as component, but want to query for enumeration constants.

See the [relationships blog](https://ajmmertens.medium.com/building-games-in-ecs-with-entity-relationships-657275ba2c6c) and [relationships manual](https://www.flecs.dev/flecs/#/docs/Relationships) for more information.
