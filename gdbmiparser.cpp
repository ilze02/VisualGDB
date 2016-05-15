#include "gdbmiparser.h"
#include "gdbwire-lib/gdbmi/gdbmi_parser.h"
#include <QMessageBox>
#include <QPlainTextEdit>


GDBMIParser::GDBMIParser(QPlainTextEdit * of)
{
    struct gdbmi_parser_callbacks callbacks = { this, parser_callback };
    parser = gdbmi_parser_create(callbacks);
    outputField = of;
}

GDBMIParser::~GDBMIParser()
{
    gdbmi_parser_destroy(parser);
}


bool GDBMIParser::doParse(char * answer)
{
    enum gdbwire_result result;

    result = gdbmi_parser_push(parser, answer);
    return (result == GDBWIRE_OK);
}

void GDBMIParser::unwrapAnswer_Output(struct gdbmi_output * output)
{
    switch (output->kind)
    {
    case GDBMI_OUTPUT_OOB:
        outputField->appendPlainText("Output parced. Output kind: Out of bound record.");
        unwrapAnswer_OOB_Record(output->variant.oob_record);
        break;
    case GDBMI_OUTPUT_RESULT:
        outputField->appendPlainText("Output parced. Output kind: Result record.");
        unwrapAnswer_Result_Record(output->variant.result_record);
        break;
    case GDBMI_OUTPUT_PROMPT:
        outputField->appendPlainText("Output parced. Output kind: Prompt.");
        break;
    case GDBMI_OUTPUT_PARSE_ERROR:
        outputField->appendPlainText("Output not parced. Error occured.");
        break;
    default:
        break;
    }
    if (output->next != NULL) unwrapAnswer_Output(output->next);
}


void GDBMIParser::unwrapAnswer_OOB_Record(struct gdbmi_oob_record * oobRecord)
{
    switch (oobRecord->kind)
    {
    case GDBMI_ASYNC:
        outputField->appendPlainText("OOB record parced. OOB kind: ASYNC.");
        unwrapAnswer_OOB_Async(oobRecord->variant.async_record);
        break;
    case GDBMI_STREAM:
        outputField->appendPlainText("OOB record parced. OOB kind: STREAM.");
        unwrapAnswer_OOB_Stream(oobRecord->variant.stream_record);
        break;
    }
}

void GDBMIParser::unwrapAnswer_OOB_Async(struct gdbmi_async_record * asyncRecord)
{
    switch (asyncRecord->kind)
    {
    case GDBMI_STATUS:
        outputField->appendPlainText("OOB Async record parced. Kind: STATUS.");
        break;
    case GDBMI_EXEC:
        outputField->appendPlainText("OOB Async record parced. Kind: EXEC.");
        break;
    case GDBMI_NOTIFY:
        outputField->appendPlainText("OOB Async record parced. Kind: NOTIFY.");
        break;
    default:
        break;
    }
    //TODO another enum
    if (asyncRecord->result) unwrapAnswer_Result(asyncRecord->result);
}

void GDBMIParser::unwrapAnswer_OOB_Stream(struct gdbmi_stream_record * streamRecord)
{
    switch (streamRecord->kind)
    {
    case GDBMI_CONSOLE:
        outputField->appendPlainText("OOB Stream record parced. Kind: CONSOLE.");
        break;
    case GDBMI_TARGET:
        outputField->appendPlainText("OOB Stream record parced. Kind: TARGET.");
        break;
    case GDBMI_LOG:
        outputField->appendPlainText("OOB Stream record parced. Kind: LOG.");
        break;
    default:
        break;
    }
    outputField->appendPlainText(streamRecord->cstring);
}

void GDBMIParser::unwrapAnswer_Result_Record(struct gdbmi_result_record * resultRecord)
{
    outputField->appendPlainText("Result parced. Token: ");
    outputField->appendPlainText(resultRecord->token);
    switch (resultRecord->result_class)
    {
    case GDBMI_DONE:
        outputField->appendPlainText("Result record parced. Result class: DONE.");
        break;
    case GDBMI_RUNNING:
        outputField->appendPlainText("Result record parced. Result class: RUNNING.");
        break;
    case GDBMI_CONNECTED:
        outputField->appendPlainText("Result record parced. Result class: CONNECTED.");
        break;
    case GDBMI_ERROR:
        outputField->appendPlainText("Result record parced. Result class: ERROR.");
        break;
    case GDBMI_EXIT:
        outputField->appendPlainText("Result record parced. Result class: EXIT.");
        break;
    case GDBMI_UNSUPPORTED:
        outputField->appendPlainText("Result record parced. Result class: UNSUPPORTED.");
        break;
    default:
        break;
    }
    if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result);
}

void GDBMIParser::unwrapAnswer_Result(struct gdbmi_result * result)
{
    switch (result->kind)
    {
    case GDBMI_CSTRING:
        outputField->appendPlainText("Result parced. Result kind: CSTRING.");
        outputField->appendPlainText("Variable + cstring: ");
        outputField->appendPlainText(result->variable);
        outputField->appendPlainText(result->variant.cstring);
        break;
    case GDBMI_TUPLE:
        outputField->appendPlainText("Result parced. Result kind: TUPLE.");
        outputField->appendPlainText("Variable: ");
        outputField->appendPlainText(result->variable);
        unwrapAnswer_Result(result->variant.result);
        break;
    case GDBMI_LIST:
        outputField->appendPlainText("Result parced. Result kind: LIST.");
        outputField->appendPlainText("Variable: ");
        outputField->appendPlainText(result->variable);
        unwrapAnswer_Result(result->variant.result);
        break;
    default:
        break;
    }
    if (result->next != NULL) unwrapAnswer_Result(result->next);
}


void parser_callback(void *context, struct gdbmi_output *output)
{
    // TODO - do something with the output
    //if (output->kind == GDBMI_OUTPUT_PARSE_ERROR) {
    //        printf("\n  Parse Error: %s\n", output->line);
    //    }
    //QMessageBox msgBox;
    //msgBox.setText("Output parced. Output kind: " + output->kind);
    //msgBox.exec();
    GDBMIParser * parser = (GDBMIParser * )context;
    parser->unwrapAnswer_Output(output);
    //parser->outputField->appendPlainText("Output parced. Output kind: " + output->kind);

    gdbmi_output_free(output);
}

