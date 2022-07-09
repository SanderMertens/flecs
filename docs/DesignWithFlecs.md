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
Flecs entities can be named. This makes it easy to identify entities in editors or while debugging, and also allows you to lookup entities by name. While this can be useful, name lookups on the world are expensive! Flecs supports relative name lookups, where you search for an entity name relative to a parent entity, which is much faster. Even so, if you do a lookup by name, it is a good idea to cache the entity handle.

## Components
Designing your components is probably the most important thing you will do in your ECS application. The reason is that if you change a component you have to update all systems that use it. Fortunately there are ways to design components that refactoring and do not negatively impact performance.

### Component Size
The most important guideline: keep your components as small and atomic. If you are choosing between a Transform component or separate Position, Rotation, Scale, Matrix components pick the latter. If you have a Turret component with a target and a rotation angle, split them up into two components.

The reasoning behind this is simple: it is very cheap (at least in Flecs) to query for multiple components at the same time. There is almost no overhead associated with adding more components to a query, due to how queries cache results.

The second reason is that it improves caching performance. If your system only needs Position, but also has to load all of the other data in Transform, you end up loading a lot of data in your cache that remains unused. This may or may not be a problem, but all else being equal it definitely won't hurt.

The third reason is that you can only split up your components so far. The number of ways in which you can combine components is infinitely larger than the number of ways in which you can split them up, and this translates, in a very practical way, into less refactoring. When you have reduced your components to atomic units of data, there simply isn't anything left to refactor.

The fourth and last reason why you want to keep your components small is code reusability. If your components are atomic units of data, they are automatically less opinionated than a component that combines more stuff. As a result, it is more likely that you find the component to work well across projects. This trickles down into the rest of your ECS code: systems written for atomic components will also be more reusable across projects.

A disadvantage of small components is that you get more of them in a project. This can make it harder to find the components a system needs, especially in large projects with hundreds of components.

### Complex component data
There is a misconception that ECS components can only contain plain data types, and should not have vectors, or more complex data structures. The reality is  more nuanced. You may find yourself often needing specialized data structures, and it is perfectly fine to store these in components.

However, you should ask yourself whether the complexity is _necessary_. For example, if you have a vector with data in a component, would that data be better off as entities? Sometimes the answers to these questions are not black and white. If the elements have individual lifecycles, they might be better stored as entities. If they live and die with the entity, maybe storing them in a vector is fine. If you always need to access the entities in a very specific order, a vector might be better.

Sometimes you might even do both, where you have a component with a vector of entity handles. Which solution works best really depends on the situation, but know that there are no "wrong" approaches here. If it works well for your application, there should be no dogmatic reasons to not do it.

## Queries
Queries are the primary method in Flecs for finding the entities for a set of components (or more specifically: a component expression). Queries are easy to use, but there a few things to keep in mind.

### Cache Your Queries
Creating a query object is expensive, iterating a query is very cheap. Because of this it really pays off to keep your query objects around, and not recreate them each time you have a need for them. Query objects can be iterated as many times as you need. You can even iterate the same query safely across threads.

### Use in/inout/out Modifiers
Flecs does some analysis based on how you read and write your components, and the more accurate the information it has is, the more efficient your code will run. If you don't specify these modifiers, Flecs assumes `[inout]`. This impacts change tracking (an `inout/out` term of a query invalidates a component when it is iterated), which in turn impacts sorting (which depends on change tracking). It can also introduce more synchronization points than strictly needed.

Note that if you use C++, and you use templates to create your queries, putting `const` before your component will automatically make it an `[in]` column.

### Annotations
You can further annotate queries with components that are not matched with the query, but that are written using ECS operations (like add, remove, set etc.). Such operations are automatically deferred and merged at the end of the frame. With annotations you can enforce merging at an earlier point, by specifying that a component modification has been queued. When Flecs sees this, it will merge back the modifications before the next read.

Annotating your queries with this information can tel. To annotate that a query writes component `Position` using `ecs_set` looks like this: `[out] :Position`. If you use `get_mut`, you could also read the component, in which case you use `[inout] :Position`. If you use `get`, use `[in] :Position`. In some cases you can't know in advance which components are going to be written. This is for example the case with the `ecs_delete` operation. To annotate a query for this use case, use `[out] :*`.

## Systems
Designing systems is probably the hardest thing to do when you are not coming from an ECS background. Object oriented code allows you to write logic that is local to a single object, whereas systems in ECS are ran for collections of similar objects. This requires a different approach towards design.

### System Scope
Try to design your systems with a single responsibility. This can sometimes be difficult, especially if you are building new features and are not exactly sure yet what the end result will look like. That is fine. It is better to start with something that works, and refine it afterwards. If you find yourself with a system that does a lot of things, don't worry. Because systems in ECS are decoupled from everything else, it is generally pretty easy to split them up.

If you manage to write systems that do only one thing, and do it well, you will have achieved ECS Zen, as you will be able to use that system across lots of projects!

### System Size
This is somewhat related to the system scope, but the question "when is a system too large" is a bit harder to answer. Sometimes a single feature requires a lot of complexity, such as a damage resolution system in a first person shooter. In these cases it can be more complex to split up the logic between different systems than it is to keep it combined. That is ok.

If you have a large, complex system the chances if reusing it across different projects is much smaller, but that is not necessarily a problem. A project will always have code that is generic and specific, so don't feel bad if you wrote a complex system that has 500 lines of code.

### System Scheduling
Flecs has the ability to run and schedule your systems for you. This has a bunch of advantages. You can annotate systems with when they are ran (see phases and pipelines) and this makes it possible to build systems that are plug and play. How does that work?

A system is basically a combination of three things: a query, a function, and ordering information. The query finds the right entities, and the function is invoked with the matched entities. The ordering information makes sure that the system is inserted in the right point in your frame. If you get this right, you can import any number of systems into your project, and you can just run them without spending any time on manually sorting them out.

The ordering information consists out of a phase (see phases and pipelines) and an implicit declaration order. Systems are ordered according to their phases first. Within a phase, they are ordered by declaration order. This may feel rigid, but is very deliberate. It prevents you from defining dependencies between systems, which can make it difficult to reuse systems across projects.

On the other hand, if you are working with an existing framework or engine, you may not have the luxury of scheduling everything yourself. The engine may for example provide you with callbacks in which you need to do certain logic. Maybe you want to build your own threading system. In those situations it can make sense to take control of running systems yourself. Sometimes you may even not use systems at all, and just use queries!

There is no right or wrong approach here. Different projects require different approaches. Fortunately Flecs makes it really easy to do both!

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

### Designing a Pipeline
If you are designing your own pipeline and you feel lost about which phases you should define, start small and expand later. A good pipeline has well defined semantics for each phase, so that you never have to think long about where a system should go. If you want to be really formal about it, create a document that describes how your components are accessed and/or modified in each phase. This will make it much easier to write systems!

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
You might wonder whether a module with lots of systems, of which only a few are used by your application makes your application slower. The answer is no. Flecs only inserts systems into the main loop that have matched with actual entities. Any imported systems that have never matched with anything remain dormant, and will not negatively affect your performance.

## Hierarchies
Hierarchies are one of the most used features of Flecs, and often useful in game development. Here are a few guidelines for using them.

### Scene graphs
The Flecs hierarchy implementation works quite well with scene graphs. Flecs has out of the box features that let you iterate hierarchies top to bottom (see the `cascade` modifier), and this comes in handy when you want to do things like hierarchically applying a transform. Storing a scene graph is a totally fine way to use Flecs hierarchies, and there are nice cross-over benefits from, for example, using prefab hierarchies.

### Keep 'em clean!
It can sometimes be tempting to use Flecs hierarchies as a fancy container for a list of entities. Don't. Or rather, you _can_, but keep the hierarchy separate from your scene graph hierarchy. Maybe it will work, but tomorrow someone may implement a feature that iterates all entities in the scene graph for some kind of visualization, and you don't want to see thousands of entities in that tree that are just there because it was convenient to use hierarchies.

To make sure your application can support multiple hierarchies, make sure that you use an explicit root object. This can be your scene, game world, level or whatever you fancy. This way you can always use that scene root entity to get all entities in your scene graph, and not run into unexpected clutter!

### Don't Sort Manually!
If you have a hierarchy and you want to apply a hierarchical transform (for example), don't sort your entities outside of the ECS. Flecs comes with the `cascade` query modifier (see the Query manual) which does this for you. The implementation of `cascade` is such that sorting is only performed when new tables are introduced to Flecs. Without going into details as to what that means: this only happens during the early stage of your application, and hardly ever afterwards. As such it is an extremely efficient way to access your entities in a depth-sorted way!
