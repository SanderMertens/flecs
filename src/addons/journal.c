/**
 * @file addons/journal.c
 * @brief Journal addon.
 */

#include "../private_api.h"

#ifdef FLECS_JOURNAL

static
char* flecs_journal_entitystr(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    char *path;
    const char *_path = ecs_get_symbol(world, entity);
    if (_path && !strchr(_path, '.')) {
        path = flecs_asprintf("#[blue]%s", _path);
    } else {
        uint32_t gen = entity >> 32;
        if (gen) {
            path = flecs_asprintf("#[normal]_%u_%u", (uint32_t)entity, gen);
        } else {
            path = flecs_asprintf("#[normal]_%u", (uint32_t)entity);
        }
    }
    return path;
}

static
char* flecs_journal_idstr(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_IS_VALUE_PAIR(id)) {
        char *first_path = flecs_journal_entitystr(world, 
            ecs_pair_first(world, id));
        char *result = flecs_asprintf("#[cyan]ecs_value_pair#[normal](%s, %u)",
            first_path, ECS_PAIR_SECOND(id));
        ecs_os_free(first_path);
        return result;
    } else if (ECS_IS_PAIR(id)) {
        char *first_path = flecs_journal_entitystr(world, 
            ecs_pair_first(world, id));
        char *second_path = flecs_journal_entitystr(world, 
            ecs_pair_second(world, id));
        char *result = flecs_asprintf("#[cyan]ecs_pair#[normal](%s, %s)",
            first_path, second_path);
        ecs_os_free(first_path);
        ecs_os_free(second_path);
        return result;
    } else if (!(id & ECS_ID_FLAGS_MASK)) {
        return flecs_journal_entitystr(world, id);
    } else {
        return ecs_id_str(world, id);
    }
}

static int flecs_journal_counter = 0;
static int flecs_journal_sp = 0;

void flecs_journal_begin(
    ecs_world_t *world,
    ecs_journal_kind_t kind,
    ecs_entity_t entity,
    ecs_type_t *add,
    ecs_type_t *remove)
{
    flecs_journal_sp ++;
    flecs_journal_counter ++;

    if (ecs_os_api.log_level_ < FLECS_JOURNAL_LOG_LEVEL) {
        return;
    }

    char *path = NULL; 
    char *var_id = NULL; 
    if (entity) {
        if (kind != EcsJournalDeleteWith && kind != EcsJournalRemoveAll) {
            path = ecs_get_path(world, entity);
            var_id = flecs_journal_entitystr(world, entity);
        } else {
            path = ecs_id_str(world, entity);
            var_id = flecs_journal_idstr(world, entity);
        }
    }

    if (kind == EcsJournalNew) {
        ecs_print(4, "%d: #[magenta]#ifndef #[normal]_var_%s", flecs_journal_counter, var_id);
        ecs_print(4, "%d: #[magenta]#define #[normal]_var_%s", flecs_journal_counter, var_id);
        ecs_print(4, "%d: #[green]ecs_entity_t %s;", flecs_journal_counter, var_id);
        ecs_print(4, "%d: #[magenta]#endif", flecs_journal_counter);
        ecs_print(4, "%d: %s = #[cyan]ecs_new#[reset](world); "
            "#[grey] // %s = new()", flecs_journal_counter, var_id, path);
    }
    if (kind == EcsJournalSetParent) {
        ecs_entity_t parent = add->array[0];
        char *parentstr = flecs_journal_entitystr(world, parent);
        ecs_print(4, "%d: #[cyan]ecs_set#[reset](world, %s, EcsParent, {%s}); "
            "#[grey] // set(%s, EcsParent, %s)", flecs_journal_counter, var_id, parentstr, 
                path, parentstr);
        ecs_os_free(parentstr);
    } else {
        if (add) {
            for (int i = 0; i < add->count; i ++) {
                char *jidstr = flecs_journal_idstr(world, add->array[i]);
                char *idstr = ecs_id_str(world, add->array[i]);
                ecs_print(4, "%d: #[cyan]ecs_add_id#[reset](world, %s, %s); "
                    "#[grey] // add(%s, %s)", flecs_journal_counter, var_id, jidstr, 
                        path, idstr);
                ecs_os_free(idstr);
                ecs_os_free(jidstr);
            }
        }
        if (remove) {
            for (int i = 0; i < remove->count; i ++) {
                char *jidstr = flecs_journal_idstr(world, remove->array[i]);
                char *idstr = ecs_id_str(world, remove->array[i]);
                ecs_print(4, "%d: #[cyan]ecs_remove_id#[reset](world, %s, %s); "
                    "#[grey] // remove(%s, %s)", flecs_journal_counter, var_id, jidstr, 
                        path, idstr);
                ecs_os_free(idstr);
                ecs_os_free(jidstr);
            }
        }

        if (kind == EcsJournalClear) {
            ecs_print(4, "%d: #[cyan]ecs_clear#[reset](world, %s); "
                "#[grey] // clear(%s)", flecs_journal_counter, var_id, path);
        } else if (kind == EcsJournalDelete) {
            ecs_print(4, "%d: #[cyan]ecs_delete#[reset](world, %s); "
                "#[grey] // delete(%s)", flecs_journal_counter, var_id, path);
        } else if (kind == EcsJournalDeleteWith) {
            ecs_print(4, "%d: #[cyan]ecs_delete_with#[reset](world, %s); "
                "#[grey] // delete_with(%s)", flecs_journal_counter, var_id, path);
        } else if (kind == EcsJournalRemoveAll) {
            ecs_print(4, "%d: #[cyan]ecs_remove_all#[reset](world, %s); "
                "#[grey] // remove_all(%s)", flecs_journal_counter, var_id, path);
        }
    }

    ecs_os_free(var_id);
    ecs_os_free(path);
    ecs_log_push();
}

void flecs_journal_end(void) {
    flecs_journal_sp --;
    ecs_assert(flecs_journal_sp >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_log_pop();
}

int flecs_journal_get_counter(void) {
    return flecs_journal_counter;
}

#endif
