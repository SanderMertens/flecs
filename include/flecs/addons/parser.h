/**
 * @file addons/parser.h
 * @brief Parser addon.
 *
 * The parser addon parses string expressions into lists of terms, and can be
 * used to construct filters, queries and types.
 */

#ifdef FLECS_PARSER

/**
 * @defgroup c_addons_parser Parser
 * @brief Query DSL parser and parsing utilities.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_PARSER_H
#define FLECS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/** Skip whitespace characters.
 * This function skips whitespace characters. Does not skip newlines.
 * 
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_whitespace(
    const char *ptr);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 * 
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_eol_and_whitespace(
    const char *ptr);

/** Parse digit.
 * This function will parse until the first non-digit character is found. The
 * provided expression must contain at least one digit character.
 * 
 * @param ptr The expression to parse.
 * @param token The output buffer.
 * @return Pointer to the first non-digit character.
 */
FLECS_API
const char* ecs_parse_digit(
    const char *ptr,
    char *token);

/** Skip whitespaces and comments.
 * This function skips whitespace characters and comments (single line, //).
 * 
 * @param ptr pointer to (potential) whitespaces/comments to skip.
 * @return pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_fluff(
    const char *ptr,
    char **last_comment);

/** Parse a single token.
 * This function can be used as simple tokenizer by other parsers.
 * 
 * @param name of program (used for logging).
 * @param expr pointer to token to parse.
 * @param ptr pointer to first character to parse.
 * @param token_out Parsed token (buffer should be ECS_MAX_TOKEN_SIZE large)
 * @return Pointer to the next token, or NULL if error occurred.
 */
FLECS_API
const char* ecs_parse_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out,
    char delim);

/** Parse term in expression.
 * This operation parses a single term in an expression and returns a pointer
 * to the next term expression.
 *
 * If the returned pointer points to the 0-terminator, the expression is fully 
 * parsed. The function would typically be called in a while loop:
 *
 * const char *ptr = expr;
 * while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) { }
 *
 * The operation does not attempt to find entity ids from the names in the
 * expression. Use the ecs_term_resolve_ids function to resolve the identifiers
 * in the parsed term.
 *
 * The returned term will in most cases contain allocated resources, which
 * should freed (or used) by the application. To free the resources for a term,
 * use the ecs_term_free function.
 *
 * The parser accepts expressions in the legacy string format.
 *
 * @param world The world.
 * @param name The name of the expression (optional, improves error logs)
 * @param expr The expression to parse (optional, improves error logs)
 * @param ptr The pointer to the current term (must be in expr).
 * @param term_out Out parameter for the term.
 * @return pointer to next term if successful, NULL if failed.
 */
FLECS_API
char* ecs_parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term_out);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FLECS_PARSER_H

/** @} */

#endif // FLECS_PARSER
