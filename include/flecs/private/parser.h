/**
 * @file addons/parser.h
 * @brief Query parser.
 *
 * The query parser parses string expressions into lists of terms, and can be
 * used to construct queries.
 */

/**
 * @defgroup c_queries Queries
 * Query DSL parser and parsing utilities.
 *
 * @{
 */

#ifndef FLECS_PARSER_H
#define FLECS_PARSER_H

/** Maximum number of extra arguments in term expression */
#define FLECS_TERM_ARG_COUNT_MAX (16)

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
const char* ecs_parse_ws(
    const char *ptr);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 *
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_ws_eol(
    const char *ptr);

/** Utility function to parse an identifier */
const char* ecs_parse_identifier(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out);

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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FLECS_PARSER_H

/** @} */
