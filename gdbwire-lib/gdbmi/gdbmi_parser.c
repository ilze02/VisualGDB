#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gdbwire-lib/logging/gdbwire_assert.h"
#include "gdbmi_grammar.h"
#include "gdbmi_parser.h"
#include "gdbwire-lib/containers/gdbwire_string.h"

/* flex prototypes used in this unit */
typedef void *yyscan_t;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

/* Lexer set/destroy buffer to parse */
extern YY_BUFFER_STATE gdbmi__scan_string(
    const char *yy_str, yyscan_t yyscanner);
extern void gdbmi__delete_buffer(YY_BUFFER_STATE state, yyscan_t yyscanner);

/* Lexer get token function */
extern int gdbmi_lex(yyscan_t yyscanner);
extern char *gdbmi_get_text(yyscan_t yyscanner);
extern void gdbmi_set_column(int column_no, yyscan_t yyscanner);

/* Lexer state create/destroy functions */
extern int gdbmi_lex_init(yyscan_t *scanner);
extern int gdbmi_lex_destroy(yyscan_t scanner);

struct gdbmi_parser {
    /* The buffer pushed into the parser from the user */
    struct gdbwire_string *buffer;
    /* The GDB/MI lexer state */
    yyscan_t mils;
    /* The GDB/MI push parser state */
    gdbmi_pstate *mips;
    /* The client parser callbacks */
    struct gdbmi_parser_callbacks callbacks;
};

struct gdbmi_parser *
gdbmi_parser_create(struct gdbmi_parser_callbacks callbacks)
{
    struct gdbmi_parser *parser;

    parser = (struct gdbmi_parser *)calloc(1, sizeof(struct gdbmi_parser));
    if (!parser) {
        return NULL;
    }

    /* Create a new buffer for the user to push parse data into */
    parser->buffer = gdbwire_string_create();
    if (!parser->buffer) {
        free(parser);
        return NULL;
    }

    /* Create a new lexer state instance */
    if (gdbmi_lex_init(&parser->mils) != 0) {
        gdbwire_string_destroy(parser->buffer);
        free(parser);
        return NULL;
    }

    /* Create a new push parser state instance */
    parser->mips = gdbmi_pstate_new();
    if (!parser->mips) {
        gdbmi_lex_destroy(parser->mils);
        gdbwire_string_destroy(parser->buffer);
        free(parser);
        return NULL;
    }

    /* Ensure that the callbacks are non null */
    if (!callbacks.gdbmi_output_callback) {
        gdbmi_pstate_delete(parser->mips);
        gdbmi_lex_destroy(parser->mils);
        gdbwire_string_destroy(parser->buffer);
        free(parser);
        return NULL;
    }

    parser->callbacks = callbacks;

    return parser;
}

void gdbmi_parser_destroy(struct gdbmi_parser *parser)
{
    if (parser) {
        /* Free the parse buffer */
        if (parser->buffer) {
            gdbwire_string_destroy(parser->buffer);
            parser->buffer = NULL;
        }

        /* Free the lexer instance */
        if (parser->mils) {
            gdbmi_lex_destroy(parser->mils);
            parser->mils = 0;
        }

        /* Free the push parser instance */
        if (parser->mips) {
            gdbmi_pstate_delete(parser->mips);
            parser->mips = NULL;
        }

        free(parser);
        parser = NULL;
    }
}

static struct gdbmi_parser_callbacks
gdbmi_parser_get_callbacks(struct gdbmi_parser *parser)
{
    return parser->callbacks;
}

/**
 * Parse a single line of output in GDB/MI format.
 *
 * The normal usage of this function is to call it over and over again with
 * more data lines and wait for it to return an mi output command.
 *
 * @param parser
 * The parser context to operate on.
 *
 * @param line
 * A line of output in GDB/MI format to be parsed.
 *
 * \return
 * GDBWIRE_OK on success or appropriate error result on failure.
 */
static enum gdbwire_result
gdbmi_parser_parse_line(struct gdbmi_parser *parser, const char *line)
{
    struct gdbmi_parser_callbacks callbacks =
        gdbmi_parser_get_callbacks(parser);
    struct gdbmi_output *output = 0;
    YY_BUFFER_STATE state = 0;
    int pattern, mi_status;

    GDBWIRE_ASSERT(parser && line);

    /* Create a new input buffer for flex. */
    state = gdbmi__scan_string(line, parser->mils);
    GDBWIRE_ASSERT(state);
    gdbmi_set_column(1, parser->mils);

    /* Iterate over all the tokens found in the scanner buffer */
    do {
        pattern = gdbmi_lex(parser->mils);
        if (pattern == 0)
            break;
        mi_status = gdbmi_push_parse(parser->mips, pattern, NULL,
            parser->mils, &output);
    } while (mi_status == YYPUSH_MORE);

    /* Free the scanners buffer */
    gdbmi__delete_buffer(state, parser->mils);

    /**
     * The push parser will return,
     * - 0 if parsing was successful (return is due to end-of-input).
     * - 1 if parsing failed because of invalid input, i.e., input
     *     that contains a syntax error or that causes YYABORT to be invoked.
     * - 2 if parsing failed due to memory exhaustion. 
     * - YYPUSH_MORE if more input is required to finish parsing the grammar. 
     * Anything besides this would be unexpected.
     *
     * The grammar is designed to accept an infinate list of GDB/MI
     * output commands. For this reason, YYPUSH_MORE is the expected
     * return value of all the calls to gdbmi_push_parse. However,
     * in reality, gdbwire only translates a line at a time from GDB.
     * When the line is finished, gdbmi_lex returns 0, and the parsing
     * is done.
     */

    // Check mi_status, will be 1 on parse error, and YYPUSH_MORE on success
    GDBWIRE_ASSERT(mi_status == 1 || mi_status == YYPUSH_MORE);

    /* Each GDB/MI line should produce an output command */
    GDBWIRE_ASSERT(output);
    output->line = strdup(line);

    callbacks.gdbmi_output_callback(callbacks.context, output);

    return GDBWIRE_OK;
}

/**
 * Get the next line available in the buffer.
 *
 * @param buffer
 * The entire buffer the user has pushed onto the gdbmi parser
 * through gdbmi_parser_push. If a line is found, the returned line
 * will be removed from this buffer.
 *
 * @param line
 * Will return as an allocated line if a line is available or NULL
 * otherwise. If this function does not return GDBWIRE_OK then ignore
 * the output of this parameter. It is the callers responsibility to
 * free the memory.
 *
 * @return
 * GDBWIRE_OK on success or appropriate error result on failure.
 */
static enum gdbwire_result
gdbmi_parser_get_next_line(struct gdbwire_string *buffer,
        struct gdbwire_string **line)
{
    enum gdbwire_result result = GDBWIRE_OK;

    GDBWIRE_ASSERT(buffer && line);

    char *data = gdbwire_string_data(buffer);
    size_t size = gdbwire_string_size(buffer);
    size_t pos = gdbwire_string_find_first_of(buffer, "\r\n");

    // Search to see if a newline has been reached in gdb/mi.
    // If a line of data has been recieved, process it.
    if (pos != size) {
        int status;

        /**
         * We have the position of the newline character from
         * gdbwire_string_find_first_of. However, the length must be
         * calculated to make a copy of the line.
         *
         * This is either pos + 1 (for \r or \n) or pos + 1 + 1 for (\r\n).
         * Check for\r\n for the special case.
         */
        size_t line_length = (data[pos] == '\r' && (pos + 1 < size) &&
                data[pos + 1] == '\n') ? pos + 2 : pos + 1;

        /**
         * - allocate the buffer
         * - append the new line
         * - append a null terminating character
         * - if successful, delete the new line found from buffer
         * - any failures cleanup and return an error
         */
        *line = gdbwire_string_create();
        GDBWIRE_ASSERT(*line);

        status = gdbwire_string_append_data(*line, data, line_length);
        GDBWIRE_ASSERT_GOTO(status == 0, result, cleanup);

        status = gdbwire_string_append_data(*line, "\0", 1);
        GDBWIRE_ASSERT_GOTO(status == 0, result, cleanup);

        status = gdbwire_string_erase(buffer, 0, line_length);
        GDBWIRE_ASSERT_GOTO(status == 0, result, cleanup);
    }

    return result;

cleanup:
    gdbwire_string_destroy(*line);
    *line = 0;
    return result;

}

enum gdbwire_result
gdbmi_parser_push(struct gdbmi_parser *parser, const char *data)
{
    return gdbmi_parser_push_data(parser, data, strlen(data));
}

enum gdbwire_result
gdbmi_parser_push_data(struct gdbmi_parser *parser, const char *data,
    size_t size)
{
    struct gdbwire_string *line = 0;
    enum gdbwire_result result = GDBWIRE_OK;

    GDBWIRE_ASSERT(parser && data);
    GDBWIRE_ASSERT(gdbwire_string_append_data(parser->buffer, data, size) == 0);

    // Loop until no more lines available
    for (;;) {
        result = gdbmi_parser_get_next_line(parser->buffer, &line);
        GDBWIRE_ASSERT_GOTO(result == GDBWIRE_OK, result, cleanup);

        if (line) {
            result = gdbmi_parser_parse_line(parser, gdbwire_string_data(line));
            gdbwire_string_destroy(line);
            line = 0;
            GDBWIRE_ASSERT_GOTO(result == GDBWIRE_OK, result, cleanup);
        } else {
            break;
        }
    }

cleanup:
    return result;
}
