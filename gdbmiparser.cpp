#include "gdbmiparser.h"
#include "gdbwire-lib/gdbmi/gdbmi_parser.h"
#include <QMessageBox>
#include <QPlainTextEdit>
#include <vdvariablelist.h>
#include <vdwindow.h>

GDBMIParser::GDBMIParser(QPlainTextEdit * of, VDWindow * p)
{
    struct gdbmi_parser_callbacks callbacks = { this, parser_callback };
    parser = gdbmi_parser_create(callbacks);
    outputField = of;
    //varList = new VDVariableList(this, process, parent);
    tokenCounter = 0;
    parent = p;
}

GDBMIParser::~GDBMIParser()
{
    gdbmi_parser_destroy(parser);
    //delete varList;
}


bool GDBMIParser::doParse(char * answer)
{
    enum gdbwire_result result;

    result = gdbmi_parser_push(parser, answer);
    return (result == GDBWIRE_OK);
}

int GDBMIParser::getToken()
{
    tokenCounter++;
    return tokenCounter;
}

void GDBMIParser::unwrapAnswer_Output(struct gdbmi_output * output)
{
    switch (output->kind)
    {
    case GDBMI_OUTPUT_OOB:
        //outputField->appendPlainText("Output parced. Output kind: Out of bound record.");
        unwrapAnswer_OOB_Record(output->variant.oob_record);
        break;
    case GDBMI_OUTPUT_RESULT:
        //outputField->appendPlainText("Output parced. Output kind: Result record.");
        unwrapAnswer_Result_Record(output->variant.result_record);
        break;
    case GDBMI_OUTPUT_PROMPT:
        //outputField->appendPlainText("(gdb)");
        break;
    case GDBMI_OUTPUT_PARSE_ERROR:
        outputField->appendPlainText("Error occured.");
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
        //outputField->appendPlainText("OOB record parced. OOB kind: ASYNC.");
        unwrapAnswer_OOB_Async(oobRecord->variant.async_record);
        break;
    case GDBMI_STREAM:
        //outputField->appendPlainText("OOB record parced. OOB kind: STREAM.");
        unwrapAnswer_OOB_Stream(oobRecord->variant.stream_record);
        break;
    }
}

void GDBMIParser::unwrapAnswer_OOB_Async(struct gdbmi_async_record * asyncRecord)
{
    switch (asyncRecord->kind)
    {
    case GDBMI_STATUS:
        //outputField->appendPlainText("OOB Async record parced. Kind: STATUS.");
        break;
    case GDBMI_EXEC:
        //outputField->appendPlainText("OOB Async record parced. Kind: EXEC.");
        break;
    case GDBMI_NOTIFY:
        //outputField->appendPlainText("OOB Async record parced. Kind: NOTIFY.");
        break;
    default:
        break;
    }
    //TODO another enum
    if (asyncRecord->result) unwrapAnswer_Result(asyncRecord->result, NULL, true);
}

void GDBMIParser::unwrapAnswer_OOB_Stream(struct gdbmi_stream_record * streamRecord)
{
    switch (streamRecord->kind)
    {
    case GDBMI_CONSOLE:
        //outputField->appendPlainText("OOB Stream record parced. Kind: CONSOLE.");
        break;
    case GDBMI_TARGET:
        //outputField->appendPlainText("OOB Stream record parced. Kind: TARGET.");
        break;
    case GDBMI_LOG:
        //outputField->appendPlainText("OOB Stream record parced. Kind: LOG.");
        break;
    default:
        break;
    }
    outputField->appendPlainText(streamRecord->cstring);
}

void GDBMIParser::unwrapAnswer_Result_Record(struct gdbmi_result_record * resultRecord)
{
    //outputField->appendPlainText("Result parced. Token: ");
    //outputField->appendPlainText(resultRecord->token);
    switch (resultRecord->result_class)
    {
    case GDBMI_DONE:
        //outputField->appendPlainText("Result record parced. Result class: DONE.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, resultRecord->token, true);
        break;
    case GDBMI_RUNNING:
        //outputField->appendPlainText("Result record parced. Result class: RUNNING.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, NULL, true);
        break;
    case GDBMI_CONNECTED:
        //outputField->appendPlainText("Result record parced. Result class: CONNECTED.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, NULL, true);
        break;
    case GDBMI_ERROR:
        outputField->appendPlainText("Result record parced. Result class: ERROR.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, NULL, false);
        break;
    case GDBMI_EXIT:
        outputField->appendPlainText("Result record parced. Result class: EXIT.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, NULL, true);
        break;
    case GDBMI_UNSUPPORTED:
        //outputField->appendPlainText("Result record parced. Result class: UNSUPPORTED.");
        if (resultRecord->result != NULL) unwrapAnswer_Result(resultRecord->result, NULL, true);
        break;
    default:
        break;
    }

}

void GDBMIParser::unwrapAnswer_Result(struct gdbmi_result * result, char * token, bool silent)
{
    bool specificUnwrap = false;
    switch (result->kind)
    {
    case GDBMI_CSTRING:
        if (token != NULL)
        {
            specificUnwrap = vdUnwrapAtributes(result, token);
        }
        if (!specificUnwrap)
        {
            if (!silent)
            {
                outputField->appendPlainText(QString("%1: %2\n").arg(result->variable).arg(result->variant.cstring));
                //outputField->appendPlainText("Result parced. Result kind: CSTRING.");
                //outputField->appendPlainText("Variable + cstring: ");
                //outputField->appendPlainText(result->variable);
                //outputField->appendPlainText(result->variant.cstring);
            }
        }
        break;
    case GDBMI_TUPLE:
        //outputField->appendPlainText("Result parced. Result kind: TUPLE.");
        //outputField->appendPlainText("Variable: ");
        //outputField->appendPlainText(result->variable);
        if(result->variant.result != NULL)
        {
            if (strcmp(result->variable, "child") == 0)
            {
                specificUnwrap = vdUnwrapChild(result->variant.result, token);
                if (specificUnwrap)
                {
                    if (result->next == NULL)
                    {
                        parent->varList->removeToken(token);
                    }
                    else
                    {
                        unwrapAnswer_Result(result->next, token, silent);
                    }
                }
            }
            if (!specificUnwrap)
            {
                unwrapAnswer_Result(result->variant.result, token, silent);
            }
        }
        break;
    case GDBMI_LIST:
        //outputField->appendPlainText("Result parced. Result kind: LIST.");
        //outputField->appendPlainText("Variable: ");
        //outputField->appendPlainText(result->variable);
        if(result->variant.result != NULL)
        {
            if (strcmp(result->variable, "locals") == 0)
            {
                specificUnwrap = vdUnwrapLocals(result->variant.result);
            }
            if (!specificUnwrap)
            {
                unwrapAnswer_Result(result->variant.result, token, silent);
            }
        }
        break;
    default:
        break;
    }
    if (!specificUnwrap && result->next != NULL) unwrapAnswer_Result(result->next, token, silent);
}

bool GDBMIParser::vdUnwrapAtributes(struct gdbmi_result * result, char * token)
{
    if (!parent->varList->updateVariableAtributes(token, result->variable, result->variant.cstring)) return false;

    if (result->next == NULL)
    {
        parent->varList->removeToken(token);
    }
    else
    {
        return(vdUnwrapAtributes(result->next, token));
    }

    return true;
}

bool GDBMIParser::vdUnwrapLocals(struct gdbmi_result * result)
{
    if (result->kind != GDBMI_CSTRING) return false;
    if (strcmp(result->variable, "name") != 0) return false;

    parent->varList->createVariable(result->variant.cstring);

    if (result->next == NULL)
    {
        parent->varList->localsParsed = true;
        parent->varList->listValues();
    }
    else
    {
        return(vdUnwrapLocals(result->next));
    }

    return true;
}

bool GDBMIParser::vdUnwrapChild(struct gdbmi_result * result, char * token)
{
    if (result->kind != GDBMI_CSTRING) return false;
    if (strcmp(result->variable, "exp") == 0)
    {
        parent->varList->createChildVariable(result->variant.cstring, token);
    }

    else
    {
        return(vdUnwrapChild(result->next, token));
    }

    return true;
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
