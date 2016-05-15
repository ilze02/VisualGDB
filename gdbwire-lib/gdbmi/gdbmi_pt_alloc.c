#include <stdlib.h>

#include "gdbmi_pt.h"
#include "gdbmi_pt_alloc.h"

/* struct gdbmi_output */
struct gdbmi_output *
gdbmi_output_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_output));
}

void
gdbmi_output_free(struct gdbmi_output *param)
{
    if (param) {
        switch (param->kind) {
            case GDBMI_OUTPUT_OOB:
                gdbmi_oob_record_free(param->variant.oob_record);
                param->variant.oob_record = NULL;
                break;
            case GDBMI_OUTPUT_RESULT:
                gdbmi_result_record_free(param->variant.result_record);
                param->variant.result_record = NULL;
                break;
            case GDBMI_OUTPUT_PROMPT:
                break;
            case GDBMI_OUTPUT_PARSE_ERROR:
                free(param->variant.error.token);
                param->variant.error.token = NULL;
                break;
        }

        free(param->line);
        param->line = 0;

        gdbmi_output_free(param->next);
        param->next = NULL;

        free(param);
        param = NULL;
    }
}

/* struct gdbmi_result_record */
struct gdbmi_result_record *
gdbmi_result_record_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_result_record));
}

void
gdbmi_result_record_free(struct gdbmi_result_record *param)
{
    if (param) {
        free(param->token);

        gdbmi_result_free(param->result);
        param->result = NULL;

        free(param);
        param = NULL;
    }
}

/* struct gdbmi_result */
struct gdbmi_result *
gdbmi_result_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_result));
}

void
gdbmi_result_free(struct gdbmi_result *param)
{
    if (param) {
        if (param->variable) {
            free(param->variable);
            param->variable = NULL;
        }

        switch (param->kind) {
            case GDBMI_CSTRING:
                if (param->variant.cstring) {
                    free(param->variant.cstring);
                    param->variant.cstring = NULL;
                }
                break;
            case GDBMI_TUPLE:
            case GDBMI_LIST:
                gdbmi_result_free(param->variant.result);
                param->variant.result = NULL;
                break;
        }

        gdbmi_result_free(param->next);
        param->next = NULL;

        free(param);
        param = NULL;
    }
}

/* struct gdbmi_oob_record */
struct gdbmi_oob_record *
gdbmi_oob_record_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_oob_record));
}

void
gdbmi_oob_record_free(struct gdbmi_oob_record *param)
{
    if (param) {
        switch(param->kind) {
            case GDBMI_ASYNC:
                gdbmi_async_record_free(param->variant.async_record);
                param->variant.async_record = NULL;
                break;
            case GDBMI_STREAM:
                gdbmi_stream_record_free(param->variant.stream_record);
                param->variant.stream_record = NULL;
                break;
        }

        free(param);
        param = NULL;
    }
}

/* struct gdbmi_async_record */
struct gdbmi_async_record *
gdbmi_async_record_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_async_record));
}

void
gdbmi_async_record_free(struct gdbmi_async_record *param)
{
    if (param) {
        free(param->token);

        gdbmi_result_free(param->result);
        param->result = NULL;

        free(param);
        param = NULL;
    }
}

/* struct gdbmi_stream_record */
struct gdbmi_stream_record *
gdbmi_stream_record_alloc(void)
{
    return calloc(1, sizeof (struct gdbmi_stream_record));
}

void
gdbmi_stream_record_free(struct gdbmi_stream_record *param)
{
    if (param) {
        if (param->cstring) {
            free(param->cstring);
            param->cstring = NULL;
        }

        free(param);
        param = NULL;
    }
}
