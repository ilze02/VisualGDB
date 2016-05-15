#ifndef GDBMIPARSER_H
#define GDBMIPARSER_H

class QString;
class QPlainTextEdit;

void parser_callback(void *context, struct gdbmi_output *output);

class GDBMIParser
{
private:
    struct gdbmi_parser *parser;

    void unwrapAnswer_OOB_Record(struct gdbmi_oob_record * oobRecord);
    void unwrapAnswer_Result_Record(struct gdbmi_result_record * resultRecord);
    void unwrapAnswer_Result(struct gdbmi_result * result);
    void unwrapAnswer_OOB_Async(struct gdbmi_async_record * asyncRecord);
    void unwrapAnswer_OOB_Stream(struct gdbmi_stream_record * streamRecord);
public:
    GDBMIParser(QPlainTextEdit * outputField);
    ~GDBMIParser();
    bool doParse(char * answer);
    QPlainTextEdit * outputField;
    void unwrapAnswer_Output(struct gdbmi_output * output);
};


#endif // GDBMIPARSER_H
