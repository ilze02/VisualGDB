#ifndef VDVARIABLELIST_H
#define VDVARIABLELIST_H

#include <QMap>

class VDVariable;
class QProcess;
class GDBMIParser;
class VDWindow;

class VDVariableList
{
private:
    QMap<QString, VDVariable*> nameMap;
    QMap<QString, VDVariable*> addressMap;
    QMap<int, VDVariable*> tokenMap;
    void addVarObject(char * name, VDVariable * parent, VDVariable * pointer);
    QProcess * process;
    GDBMIParser * parser;
    VDWindow * window;
public:
    VDVariableList(GDBMIParser * parser, QProcess * process, VDWindow * w);
    ~VDVariableList();
    bool localsParsed;
    void createVariable(char * name);
    void createChildVariable(char * name, char * token);
    bool updateVariableAtributes(char * token, char * atribute, char * value);
    void getVariableMembers(VDVariable * var, char * keyword);
    void getVariablePoint(VDVariable * var);
    void resolveExistingAddress(VDVariable * var);
    void resolve(VDVariable * var);
    void listValues();
    void removeToken(char * token);
    void removeVariable(VDVariable * var);
    void makeGraphFile();
    //makeGraph();
};

#endif // VDVARIABLELIST_H
