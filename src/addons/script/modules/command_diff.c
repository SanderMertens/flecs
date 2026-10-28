/**
 * @file addons/script/modules/command_diff.c
 * @brief Line based diff and patch for script code.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_COMMAND
#include "command.h"

#define FLECS_COMMAND_DIFF_CONTEXT (3)
#define FLECS_COMMAND_DIFF_MAX_CELLS (16 * 1024 * 1024)

typedef struct {
    const char *ptr;
    int32_t len;
} flecs_command_line_t;

typedef enum {
    FlecsDiffKeep,
    FlecsDiffRemove,
    FlecsDiffAdd
} flecs_command_diff_kind_t;

typedef struct {
    flecs_command_diff_kind_t kind;
    int32_t old_line;
    int32_t new_line;
} flecs_command_diff_op_t;

typedef struct {
    char *anchor;
    ecs_vec_t old_lines;
    ecs_vec_t new_lines;
} flecs_command_hunk_t;

static void flecs_command_split(
    const char *code,
    ecs_vec_t *lines)
{
    ecs_vec_init_t(NULL, lines, flecs_command_line_t, 0);
    if (!code) {
        return;
    }

    const char *ptr = code;
    while (ptr[0]) {
        const char *end = strchr(ptr, '\n');
        if (!end) {
            end = ptr + ecs_os_strlen(ptr);
        }

        flecs_command_line_t *l = ecs_vec_append_t(
            NULL, lines, flecs_command_line_t);
        l->ptr = ptr;
        l->len = flecs_ito(int32_t, end - ptr);

        if (!end[0]) {
            break;
        }

        ptr = end + 1;
    }
}

static bool flecs_command_line_eq(
    const flecs_command_line_t *a,
    const flecs_command_line_t *b)
{
    return a->len == b->len && !ecs_os_memcmp(a->ptr, b->ptr, a->len);
}

static bool flecs_command_line_eq_trimmed(
    const flecs_command_line_t *a,
    const flecs_command_line_t *b)
{
    const char *a_start = a->ptr, *a_end = a->ptr + a->len;
    const char *b_start = b->ptr, *b_end = b->ptr + b->len;
    while (a_start < a_end && isspace(a_start[0])) a_start ++;
    while (a_end > a_start && isspace(a_end[-1])) a_end --;
    while (b_start < b_end && isspace(b_start[0])) b_start ++;
    while (b_end > b_start && isspace(b_end[-1])) b_end --;
    return (a_end - a_start) == (b_end - b_start) &&
        !ecs_os_memcmp(a_start, b_start, (ecs_size_t)(a_end - a_start));
}

static void flecs_command_diff_ops(
    const flecs_command_line_t *a,
    int32_t a_count,
    const flecs_command_line_t *b,
    int32_t b_count,
    ecs_vec_t *ops)
{
    int32_t prefix = 0;
    while (prefix < a_count && prefix < b_count &&
        flecs_command_line_eq(&a[prefix], &b[prefix]))
    {
        prefix ++;
    }

    int32_t suffix = 0;
    while (suffix < (a_count - prefix) && suffix < (b_count - prefix) &&
        flecs_command_line_eq(&a[a_count - 1 - suffix], &b[b_count - 1 - suffix]))
    {
        suffix ++;
    }

    int32_t i, j;
    for (i = 0; i < prefix; i ++) {
        ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
            (flecs_command_diff_op_t){ FlecsDiffKeep, i, i };
    }

    int32_t n = a_count - prefix - suffix;
    int32_t m = b_count - prefix - suffix;
    const flecs_command_line_t *ma = &a[prefix];
    const flecs_command_line_t *mb = &b[prefix];

    if (((int64_t)(n + 1) * (int64_t)(m + 1)) > FLECS_COMMAND_DIFF_MAX_CELLS) {
        for (i = 0; i < n; i ++) {
            ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
                (flecs_command_diff_op_t){ FlecsDiffRemove,
                    prefix + i, prefix };
        }
        for (j = 0; j < m; j ++) {
            ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
                (flecs_command_diff_op_t){ FlecsDiffAdd,
                    prefix + n, prefix + j };
        }
    } else if (n || m) {
        int32_t *lcs = ecs_os_calloc_n(int32_t, (n + 1) * (m + 1));
        for (i = n - 1; i >= 0; i --) {
            for (j = m - 1; j >= 0; j --) {
                if (flecs_command_line_eq(&ma[i], &mb[j])) {
                    lcs[i * (m + 1) + j] = lcs[(i + 1) * (m + 1) + j + 1] + 1;
                } else {
                    int32_t down = lcs[(i + 1) * (m + 1) + j];
                    int32_t right = lcs[i * (m + 1) + j + 1];
                    lcs[i * (m + 1) + j] = down > right ? down : right;
                }
            }
        }

        i = 0; j = 0;
        while (i < n || j < m) {
            if (i < n && j < m && flecs_command_line_eq(&ma[i], &mb[j])) {
                ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
                    (flecs_command_diff_op_t){ FlecsDiffKeep,
                        prefix + i, prefix + j };
                i ++; j ++;
            } else if (i < n && (j == m || lcs[(i + 1) * (m + 1) + j] >=
                lcs[i * (m + 1) + j + 1]))
            {
                ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
                    (flecs_command_diff_op_t){ FlecsDiffRemove,
                        prefix + i, prefix + j };
                i ++;
            } else {
                ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
                    (flecs_command_diff_op_t){ FlecsDiffAdd,
                        prefix + i, prefix + j };
                j ++;
            }
        }

        ecs_os_free(lcs);
    }

    for (i = 0; i < suffix; i ++) {
        ecs_vec_append_t(NULL, ops, flecs_command_diff_op_t)[0] =
            (flecs_command_diff_op_t){ FlecsDiffKeep,
                a_count - suffix + i, b_count - suffix + i };
    }
}

static void flecs_command_diff_line(
    ecs_strbuf_t *buf,
    char prefix,
    const flecs_command_line_t *line)
{
    ecs_strbuf_appendch(buf, prefix);
    ecs_strbuf_appendstrn(buf, line->ptr, line->len);
    ecs_strbuf_appendch(buf, '\n');
}

char* flecs_command_diff(
    const char *from,
    const char *to)
{
    ecs_vec_t a, b, ops;
    flecs_command_split(from, &a);
    flecs_command_split(to, &b);
    ecs_vec_init_t(NULL, &ops, flecs_command_diff_op_t, 0);

    const flecs_command_line_t *a_lines = ecs_vec_first(&a);
    const flecs_command_line_t *b_lines = ecs_vec_first(&b);
    flecs_command_diff_ops(a_lines, ecs_vec_count(&a),
        b_lines, ecs_vec_count(&b), &ops);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    const flecs_command_diff_op_t *op = ecs_vec_first(&ops);
    int32_t i, count = ecs_vec_count(&ops);
    for (i = 0; i < count; ) {
        if (op[i].kind == FlecsDiffKeep) {
            i ++;
            continue;
        }

        int32_t start = i - FLECS_COMMAND_DIFF_CONTEXT;
        if (start < 0) {
            start = 0;
        }

        int32_t k, last_change = i;
        for (k = i; k < count; k ++) {
            if (op[k].kind != FlecsDiffKeep) {
                last_change = k;
            } else if ((k - last_change) > 2 * FLECS_COMMAND_DIFF_CONTEXT) {
                break;
            }
        }

        int32_t end = last_change + 1 + FLECS_COMMAND_DIFF_CONTEXT;
        if (end > count) {
            end = count;
        }

        int32_t old_start = op[start].old_line, old_count = 0;
        int32_t new_start = op[start].new_line, new_count = 0;
        for (k = start; k < end; k ++) {
            old_count += op[k].kind != FlecsDiffAdd;
            new_count += op[k].kind != FlecsDiffRemove;
        }

        ecs_strbuf_append(&buf, "@@ -%d,%d +%d,%d @@\n",
            old_count ? old_start + 1 : old_start, old_count,
            new_count ? new_start + 1 : new_start, new_count);

        for (k = start; k < end; k ++) {
            if (op[k].kind == FlecsDiffKeep) {
                flecs_command_diff_line(&buf, ' ', &a_lines[op[k].old_line]);
            } else if (op[k].kind == FlecsDiffRemove) {
                flecs_command_diff_line(&buf, '-', &a_lines[op[k].old_line]);
            } else {
                flecs_command_diff_line(&buf, '+', &b_lines[op[k].new_line]);
            }
        }

        i = end;
    }

    ecs_vec_fini_t(NULL, &ops, flecs_command_diff_op_t);
    ecs_vec_fini_t(NULL, &a, flecs_command_line_t);
    ecs_vec_fini_t(NULL, &b, flecs_command_line_t);

    char *result = ecs_strbuf_get(&buf);
    if (!result) {
        result = ecs_os_strdup("");
    }

    return result;
}

static void flecs_command_hunks_fini(
    ecs_vec_t *hunks)
{
    flecs_command_hunk_t *h = ecs_vec_first(hunks);
    int32_t i, count = ecs_vec_count(hunks);
    for (i = 0; i < count; i ++) {
        ecs_os_free(h[i].anchor);
        ecs_vec_fini_t(NULL, &h[i].old_lines, flecs_command_line_t);
        ecs_vec_fini_t(NULL, &h[i].new_lines, flecs_command_line_t);
    }

    ecs_vec_fini_t(NULL, hunks, flecs_command_hunk_t);
}

static flecs_command_hunk_t* flecs_command_hunk_new(
    ecs_vec_t *hunks,
    const char *header,
    int32_t header_len)
{
    flecs_command_hunk_t *h = ecs_vec_append_t(
        NULL, hunks, flecs_command_hunk_t);
    ecs_os_zeromem(h);
    ecs_vec_init_t(NULL, &h->old_lines, flecs_command_line_t, 0);
    ecs_vec_init_t(NULL, &h->new_lines, flecs_command_line_t, 0);

    if (!header) {
        return h;
    }

    const char *ptr = header, *end = header + header_len;
    while (ptr < end && isspace(ptr[0])) ptr ++;

    if (ptr < end && ptr[0] == '-' && isdigit(ptr[1])) {
        const char *close = strstr(ptr, "@@");
        if (close && close < end) {
            ptr = close + 2;
        } else {
            ptr = end;
        }
    }

    while (ptr < end && isspace(ptr[0])) ptr ++;
    while (end > ptr && isspace(end[-1])) end --;
    if (end - ptr >= 2 && end[-1] == '@' && end[-2] == '@') {
        end -= 2;
        while (end > ptr && isspace(end[-1])) end --;
    }

    if (end > ptr) {
        h->anchor = flecs_asprintf("%.*s",
            flecs_ito(int32_t, end - ptr), ptr);
    }

    return h;
}

static int flecs_command_patch_parse(
    const char *patch,
    ecs_vec_t *hunks)
{
    ecs_vec_t lines;
    flecs_command_split(patch, &lines);
    flecs_command_line_t *l = ecs_vec_first(&lines);
    int32_t i, count = ecs_vec_count(&lines);
    flecs_command_hunk_t *hunk = NULL;
    bool has_headers = false;

    for (i = 0; i < count; i ++) {
        if (l[i].len >= 2 && !ecs_os_strncmp(l[i].ptr, "@@", 2)) {
            has_headers = true;
            break;
        }
    }

    for (i = 0; i < count; i ++) {
        const flecs_command_line_t *line = &l[i];
        if (line->len >= 2 && !ecs_os_strncmp(line->ptr, "@@", 2)) {
            hunk = flecs_command_hunk_new(
                hunks, line->ptr + 2, line->len - 2);
            continue;
        }

        if (!hunk) {
            if (has_headers || (line->len >= 3 &&
                (!ecs_os_strncmp(line->ptr, "---", 3) ||
                 !ecs_os_strncmp(line->ptr, "+++", 3))))
            {
                continue;
            }

            hunk = flecs_command_hunk_new(hunks, NULL, 0);
        }

        flecs_command_line_t content = { line->ptr + 1, line->len - 1 };
        char prefix = line->len ? line->ptr[0] : ' ';
        if (!line->len) {
            content.ptr = line->ptr;
            content.len = 0;
        }

        if (prefix == ' ') {
            ecs_vec_append_t(NULL, &hunk->old_lines,
                flecs_command_line_t)[0] = content;
            ecs_vec_append_t(NULL, &hunk->new_lines,
                flecs_command_line_t)[0] = content;
        } else if (prefix == '-') {
            ecs_vec_append_t(NULL, &hunk->old_lines,
                flecs_command_line_t)[0] = content;
        } else if (prefix == '+') {
            ecs_vec_append_t(NULL, &hunk->new_lines,
                flecs_command_line_t)[0] = content;
        } else if (prefix == '\\') {
        } else {
            ecs_err("invalid line %d in patch, expected ' ', '-' or '+': '%.*s'",
                i + 1, line->len, line->ptr);
            ecs_vec_fini_t(NULL, &lines, flecs_command_line_t);
            return -1;
        }
    }

    ecs_vec_fini_t(NULL, &lines, flecs_command_line_t);

    if (!ecs_vec_count(hunks)) {
        ecs_err("patch has no hunks");
        return -1;
    }

    return 0;
}

static bool flecs_command_line_contains(
    const flecs_command_line_t *line,
    const char *text)
{
    int32_t i, len = ecs_os_strlen(text);
    for (i = 0; i + len <= line->len; i ++) {
        if (!ecs_os_memcmp(&line->ptr[i], text, len)) {
            return true;
        }
    }

    return false;
}

static bool flecs_command_hunk_match_at(
    const flecs_command_line_t *code,
    int32_t pos,
    const flecs_command_line_t *old,
    int32_t old_count)
{
    int32_t k;
    for (k = 0; k < old_count; k ++) {
        if (!flecs_command_line_eq(&code[pos + k], &old[k])) {
            return false;
        }
    }

    return true;
}

static void flecs_command_hunk_near_match(
    int32_t hunk_index,
    const flecs_command_line_t *code,
    int32_t code_count,
    int32_t start,
    const flecs_command_line_t *old,
    int32_t old_count)
{
    int32_t best = -1, best_score = 0, pos;
    for (pos = start; pos < code_count; pos ++) {
        int32_t k, score = 0;
        for (k = 0; k < old_count && (pos + k) < code_count; k ++) {
            if (flecs_command_line_eq(&code[pos + k], &old[k])) {
                score += 2;
            } else if (flecs_command_line_eq_trimmed(&code[pos + k], &old[k])) {
                score += 1;
            }
        }

        if (score > best_score) {
            best_score = score;
            best = pos;
        }
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_append(&buf, "hunk %d does not match", hunk_index + 1);
    if (best == -1) {
        ecs_strbuf_appendlit(&buf, ", no similar lines found");
    } else {
        ecs_strbuf_append(&buf, ", closest match at line %d:", best + 1);
        int32_t k;
        for (k = 0; k < old_count; k ++) {
            if ((best + k) >= code_count) {
                ecs_strbuf_append(&buf, "\n  line %d: expected '%.*s', "
                    "found end of script", best + k + 1, old[k].len, old[k].ptr);
                break;
            }

            const flecs_command_line_t *actual = &code[best + k];
            if (!flecs_command_line_eq(actual, &old[k])) {
                ecs_strbuf_append(&buf, "\n  line %d: expected '%.*s', "
                    "found '%.*s'", best + k + 1, old[k].len, old[k].ptr,
                    actual->len, actual->ptr);
            }
        }
    }

    char *msg = ecs_strbuf_get(&buf);
    ecs_err("%s", msg);
    ecs_os_free(msg);
}

static void flecs_command_append_lines(
    ecs_vec_t *out,
    const flecs_command_line_t *lines,
    int32_t count)
{
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(NULL, out, flecs_command_line_t)[0] = lines[i];
    }
}

char* flecs_command_patch(
    const char *code,
    const char *patch,
    ecs_vec_t *changed_lines)
{
    char *result = NULL;
    ecs_vec_t hunks, lines, out;
    ecs_vec_init_t(NULL, &hunks, flecs_command_hunk_t, 0);
    ecs_vec_init_t(NULL, &out, flecs_command_line_t, 0);
    flecs_command_split(code, &lines);

    if (flecs_command_patch_parse(patch, &hunks)) {
        goto done;
    }

    const flecs_command_line_t *l = ecs_vec_first(&lines);
    int32_t line_count = ecs_vec_count(&lines);
    flecs_command_hunk_t *h = ecs_vec_first(&hunks);
    int32_t i, pos = 0, hunk_count = ecs_vec_count(&hunks);

    for (i = 0; i < hunk_count; i ++) {
        const flecs_command_line_t *old = ecs_vec_first(&h[i].old_lines);
        int32_t old_count = ecs_vec_count(&h[i].old_lines);
        const flecs_command_line_t *new_lines = ecs_vec_first(&h[i].new_lines);
        int32_t new_count = ecs_vec_count(&h[i].new_lines);
        int32_t start = pos, match = -1;

        if (h[i].anchor) {
            int32_t a;
            for (a = pos; a < line_count; a ++) {
                if (flecs_command_line_contains(&l[a], h[i].anchor)) {
                    break;
                }
            }

            if (a == line_count) {
                ecs_err("anchor '%s' of hunk %d not found", h[i].anchor, i + 1);
                goto done;
            }

            start = a;
        }

        if (!old_count) {
            if (h[i].anchor) {
                match = start + 1;
            } else if (!line_count) {
                match = 0;
            } else {
                ecs_err("hunk %d has no context lines and no anchor", i + 1);
                goto done;
            }
        } else {
            int32_t m, matches = 0;
            for (m = start; m + old_count <= line_count; m ++) {
                if (flecs_command_hunk_match_at(l, m, old, old_count)) {
                    if (!matches) {
                        match = m;
                    }
                    matches ++;
                }
            }

            if (!matches) {
                flecs_command_hunk_near_match(
                    i, l, line_count, start, old, old_count);
                goto done;
            }

            if (matches > 1) {
                ecs_err("hunk %d matches %d times, add context lines or an "
                    "anchor to make it unique", i + 1, matches);
                goto done;
            }
        }

        flecs_command_append_lines(&out, &l[pos], match - pos);
        int32_t first_line = ecs_vec_count(&out) + 1;
        flecs_command_append_lines(&out, new_lines, new_count);
        if (changed_lines) {
            int32_t k, last_line = ecs_vec_count(&out);
            if (last_line < first_line) {
                last_line = first_line;
            }
            for (k = first_line; k <= last_line; k ++) {
                ecs_vec_append_t(NULL, changed_lines, int32_t)[0] = k;
            }
        }

        pos = match + old_count;
    }

    flecs_command_append_lines(&out, &l[pos], line_count - pos);

    {
        ecs_strbuf_t buf = ECS_STRBUF_INIT;
        const flecs_command_line_t *o = ecs_vec_first(&out);
        int32_t k, out_count = ecs_vec_count(&out);
        bool trailing_newline = code && code[0] &&
            code[ecs_os_strlen(code) - 1] == '\n';
        for (k = 0; k < out_count; k ++) {
            ecs_strbuf_appendstrn(&buf, o[k].ptr, o[k].len);
            if (k != (out_count - 1) || trailing_newline || !line_count) {
                ecs_strbuf_appendch(&buf, '\n');
            }
        }

        result = ecs_strbuf_get(&buf);
        if (!result) {
            result = ecs_os_strdup("");
        }
    }

done:
    flecs_command_hunks_fini(&hunks);
    ecs_vec_fini_t(NULL, &lines, flecs_command_line_t);
    ecs_vec_fini_t(NULL, &out, flecs_command_line_t);
    return result;
}

#endif
