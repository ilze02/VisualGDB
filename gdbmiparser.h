#ifndef GDBMIPARSER_H
#define GDBMIPARSER_H

class QString;
class QPlainTextEdit;
class VDVariableList;
class QProcess;
class VDWindow;

void parser_callback(void *context, struct gdbmi_output *output);

class GDBMIParser
{
private:
    struct gdbmi_parser *parser;

    void unwrapAnswer_OOB_Record(struct gdbmi_oob_record * oobRecord);
    void unwrapAnswer_Result_Record(struct gdbmi_result_record * resultRecord);
    void unwrapAnswer_Result(struct gdbmi_result * result, char * token);
    void unwrapAnswer_OOB_Async(struct gdbmi_async_record * asyncRecord);
    void unwrapAnswer_OOB_Stream(struct gdbmi_stream_record * streamRecord);
    bool vdUnwrapLocals(struct gdbmi_result * result);
    bool vdUnwrapChild(struct gdbmi_result * result, char * token);
    bool vdUnwrapAtributes(struct gdbmi_result * result, char * token);
    int tokenCounter;
    VDVariableList * varList;
public:
    GDBMIParser(QPlainTextEdit * outputField, QProcess * process, VDWindow * parent);
    ~GDBMIParser();
    bool doParse(char * answer);
    QPlainTextEdit * outputField;
    void unwrapAnswer_Output(struct gdbmi_output * output);
    int getToken();
};


#endif // GDBMIPARSER_H
