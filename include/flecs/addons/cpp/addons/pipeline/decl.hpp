#pragma once

namespace flecs {

class pipeline;

/* Builtin pipeline tags */
static const flecs::entity_t PreFrame = EcsPreFrame;
static const flecs::entity_t OnLoad = EcsOnLoad;
static const flecs::entity_t PostLoad = EcsPostLoad;
static const flecs::entity_t PreUpdate = EcsPreUpdate;
static const flecs::entity_t OnUpdate = EcsOnUpdate;
static const flecs::entity_t OnValidate = EcsOnValidate;
static const flecs::entity_t PostUpdate = EcsPostUpdate;
static const flecs::entity_t PreStore = EcsPreStore;
static const flecs::entity_t OnStore = EcsOnStore;
static const flecs::entity_t PostFrame = EcsPostFrame;

template<typename T>
struct pipeline_m : mixin<T> { };

/** Pipeline mixin for flecs::world. */
template<>
struct pipeline_m<flecs::world> : mixin<flecs::world> {
  /** Initialize mixin. */
  void init() { }

  /** Create a new pipeline.
   *
   * @tparam Args Arguments to pass into the constructor of flecs::system.
   * @return System builder.
   */
  template <typename... Args>
  flecs::pipeline pipeline(Args &&... args) const;

  /** Set pipeline.
   * @see ecs_set_pipeline
   */
  void set_pipeline(const flecs::pipeline& pip) const;

  /** Get pipeline.
   * @see ecs_get_pipeline
   */
  flecs::pipeline get_pipeline() const;

  /** Progress world one tick.
   * @see ecs_progress
   */
  bool progress(FLECS_FLOAT delta_time = 0.0) const;

  /** Run pipeline.
   * @see ecs_run_pipeline
   */
  void run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time = 0.0) const;

  /** Set timescale
   * @see ecs_set_time_scale
   */
  void set_time_scale(FLECS_FLOAT mul) const;

  /** Get timescale
   * @see ecs_get_time_scale
   */
  FLECS_FLOAT get_time_scale() const;

  /** Get tick
   * @return Monotonically increasing frame count.
   */
  int32_t get_tick() const;

  /** Set target FPS
   * @see ecs_set_target_fps
   */
  void set_target_fps(FLECS_FLOAT target_fps) const;

  /** Get target FPS
   * @return Configured frames per second.
   */
  FLECS_FLOAT get_target_fps() const;

  /** Reset simulation clock.
   * @see ecs_reset_clock
   */
  void reset_clock() const;

  /** Deactivate systems.
   * @see ecs_deactivate_systems.
   */
  void deactivate_systems() const;

  /** Set number of threads.
   * @see ecs_set_threads
   */
  void set_threads(int32_t threads) const;

  /** Set number of threads.
   * @see ecs_get_threads
   */
  int32_t get_threads() const;
};

using pipeline_m_world = pipeline_m<flecs::world>;

}
