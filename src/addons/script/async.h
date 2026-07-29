/**
 * @file addons/script/async.h
 * @brief Async/await support for Flecs script.
 */

#ifndef FLECS_SCRIPT_ASYNC_PRIVATE_H
#define FLECS_SCRIPT_ASYNC_PRIVATE_H

#ifdef FLECS_SCRIPT_ASYNC

/* Async statement evaluation, invoked by the script runner */

int flecs_script_step_await(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt);

int flecs_script_step_try(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

/* Enter a catch clause if the frame is a try block that catches the pending
 * thrown error. Returns false if the error should continue to unwind. */
bool flecs_script_try_catch(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

void flecs_script_throw_clear(
    ecs_script_runner_t *r);

void flecs_script_report_throw(
    ecs_script_runner_t *r);

void flecs_script_async_import(
    ecs_world_t *world);

#endif // FLECS_SCRIPT_ASYNC
#endif // FLECS_SCRIPT_ASYNC_PRIVATE_H
