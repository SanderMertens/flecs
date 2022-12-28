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
        path = ecs_asprintf("#[blue]%s", _path);
    } else {
        uint32_t gen = entity >> 32;
        if (gen) {
            path = ecs_asprintf("#[normal]_%u_%u", (uint32_t)entity, gen);
        } else {
            path = ecs_asprintf("#[normal]_%u", (uint32_t)entity);
        }
    }
    return path;
}

static
char* flecs_journal_idstr(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        char *first_path = flecs_journal_entitystr(world, 
            ecs_pair_first(world, id));
        char *second_path = flecs_journal_entitystr(world, 
            ecs_pair_second(world, id));
        char *result = ecs_asprintf("#[cyan]ecs_pair#[normal](%s, %s)",
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

static int flecs_journal_sp = 0;

void flecs_journal_begin(
    ecs_world_t *world,
    ecs_journal_kind_t kind,
    ecs_entity_t entity,
    ecs_type_t *add,
    ecs_type_t *remove)
{
    flecs_journal_sp ++;

    if (ecs_os_api.log_level_ < FLECS_JOURNAL_LOG_LEVEL) {
        return;
    }

    char *path = NULL; 
    char *var_id = NULL; 
    if (entity) {
        path = ecs_get_fullpath(world, entity);
        var_id = flecs_journal_entitystr(world, entity);
    }

    if (kind == EcsJournalNew) {
        ecs_print(4, "#[magenta]#ifndef #[normal]_var_%s", var_id);
        ecs_print(4, "#[magenta]#define #[normal]_var_%s", var_id);
        ecs_print(4, "#[green]ecs_entity_t %s;", var_id);
        ecs_print(4, "#[magenta]#endif");
        ecs_print(4, "%s = #[cyan]ecs_new_id#[reset](world); "
            "#[grey] // %s = new()", var_id, path);
    }
    if (add) {
        for (int i = 0; i < add->count; i ++) {
            char *jidstr = flecs_journal_idstr(world, add->array[i]);
            char *idstr = ecs_id_str(world, add->array[i]);
            ecs_print(4, "#[cyan]ecs_add_id#[reset](world, %s, %s); "
                "#[grey] // add(%s, %s)", var_id, jidstr, 
                    path, idstr);
            ecs_os_free(idstr);
            ecs_os_free(jidstr);
        }
    }
    if (remove) {
        for (int i = 0; i < remove->count; i ++) {
            char *jidstr = flecs_journal_idstr(world, remove->array[i]);
            char *idstr = ecs_id_str(world, remove->array[i]);
            ecs_print(4, "#[cyan]ecs_remove_id#[reset](world, %s, %s); "
                "#[grey] // remove(%s, %s)", var_id, jidstr, 
                    path, idstr);
            ecs_os_free(idstr);
            ecs_os_free(jidstr);
        }
    }
    if (kind == EcsJournalClear) {
        ecs_print(4, "#[cyan]ecs_clear#[reset](world, %s); "
            "#[grey] // clear(%s)", var_id, path);
    } else if (kind == EcsJournalDelete) {
        ecs_print(4, "#[cyan]ecs_delete#[reset](world, %s); "
            "#[grey] // delete(%s)", var_id, path);
    } else if (kind == EcsJournalDeleteWith) {
        ecs_print(4, "#[cyan]ecs_delete_with#[reset](world, %s); "
            "#[grey] // delete_with(%s)", var_id, path);
    } else if (kind == EcsJournalRemoveAll) {
        ecs_print(4, "#[cyan]ecs_remove_all#[reset](world, %s); "
            "#[grey] // remove_all(%s)", var_id, path);
    } else if (kind == EcsJournalTableEvents) {
        ecs_print(4, "#[cyan]ecs_run_aperiodic#[reset](world, "
            "EcsAperiodicEmptyTables);");
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

#endif
