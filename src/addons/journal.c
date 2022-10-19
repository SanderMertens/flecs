#include "../private_api.h"

#ifdef FLECS_JOURNAL

char* flecs_journal_entitystr(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    char *path;
    const char *_path = ecs_get_symbol(world, entity);
    if (_path) {
        path = ecs_asprintf("#[blue]%s#[reset]", _path);
    } else {
        path = ecs_asprintf("#[normal]_%u#[reset]", (uint32_t)entity);
    }
    return path;
}

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
static int flecs_journal_count = 0;

void flecs_journal_begin(
    ecs_world_t *world,
    ecs_journal_kind_t kind,
    ecs_entity_t entity,
    ecs_type_t *add,
    ecs_type_t *remove)
{
    if (ecs_os_api.log_level_ < FLECS_JOURNAL_LOG_LEVEL) {
        return;
    }
    if (flecs_journal_sp) {
        return;
    }

    flecs_journal_sp ++;

    char *path = ecs_get_fullpath(world, entity);
    char *var_id = flecs_journal_entitystr(world, entity);
    if (kind == EcsJournalNew) {
        ecs_print(4, "#[magenta]#ifndef #[normal]_var_%s", var_id);
        ecs_print(4, "#[magenta]#define #[normal]_var_%s", var_id);
        ecs_print(4, "#[green]ecs_entity_t#[reset] %s;", var_id);
        ecs_print(4, "#[magenta]#endif#[reset]");
        ecs_print(4, "%s = #[cyan]ecs_new_id#[reset](world); "
            "#[grey] // #%d: %s = new()", var_id, flecs_journal_count++, path);
    }
    if (add) {
        for (int i = 0; i < add->count; i ++) {
            char *jidstr = flecs_journal_idstr(world, add->array[i]);
            char *idstr = ecs_id_str(world, add->array[i]);
            ecs_print(4, "#[cyan]ecs_add_id#[reset](world, %s, %s); "
                "#[grey] // #%d: add(%s, %s)", var_id, jidstr, 
                    flecs_journal_count++, path, idstr);
            ecs_os_free(idstr);
            ecs_os_free(jidstr);
        }
    }
    if (remove) {
        for (int i = 0; i < remove->count; i ++) {
            char *jidstr = flecs_journal_idstr(world, remove->array[i]);
            char *idstr = ecs_id_str(world, remove->array[i]);
            ecs_print(4, "#[cyan]ecs_remove_id#[reset](world, %s, %s); "
                "#[grey] // #%d: remove(%s, %s)", var_id, jidstr, 
                    flecs_journal_count++, path, idstr);
            ecs_os_free(idstr);
            ecs_os_free(jidstr);
        }
    }
    if (kind == EcsJournalClear) {
        ecs_print(4, "#[cyan]ecs_clear#[reset](world, %s); "
            "#[grey] // #%d: %s", var_id, flecs_journal_count++, path);
    } else if (kind == EcsJournalDelete) {
        ecs_print(4, "#[cyan]ecs_delete#[reset](world, %s); "
            "#[grey] // #%d: %s", var_id, flecs_journal_count++, path);
    }
    ecs_os_free(var_id);
    ecs_os_free(path);
    ecs_log_push();
}

void flecs_journal_end(void) {
    if (flecs_journal_sp) {
        flecs_journal_sp --;
        ecs_log_pop();
    }    
}

#endif
