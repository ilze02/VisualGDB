#include <vdvariablelist.h>
#include <vdvariable.h>
#include <gdbmiparser.h>
#include <QProcess>
#include <gdbmiparser.h>
#include <QMessageBox>
#include <QString>
#include <QList>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <vdwindow.h>
#include <gdbmiwriter.h>
//#include <gvc.h>
//#include "gvplugin.h"

VDVariableList::VDVariableList(GDBMIParser * pa, GDBMIWriter * wr, VDWindow * w)
{
    parser = pa;
    writer = wr;
    localsParsed = false;
    window = w;
}

VDVariableList::~VDVariableList()
{
    //writer->writeVarDeleteAll();
}

void VDVariableList::createVariable(char * n)
{
    if (!nameMap.contains(n))
    {
        addVarObject(n, NULL, NULL);
    }
}

void VDVariableList::createChildVariable(char * n, char * t)
{
    int token = 0;
    if (t != NULL) token = atoi(t);
    if (token == 0) return;

    VDVariable * parent = tokenMap.value(token);
    if (parent->childrenToken != token) return;

    if (strcmp(n, "private") == 0 || strcmp(n, "public") == 0 || strcmp(n, "protected") == 0) //this is not a member
    {
        //removeToken(t);
        getVariableMembers(parent, n);
        return;
    }

    //QString fullName = QString("%1.%2").arg(parent->name).arg(n);
    QString fullNameForMap;
    if (parent->isNamed)
    {
        fullNameForMap = QString("%1.%2").arg(parent->name).arg(n);
    }
    else
    {
        fullNameForMap = QString("[%1.%2]").arg(parent->name).arg(n);
    }

    if (!nameMap.contains(fullNameForMap))
    {
        addVarObject(n, parent, NULL);
    }
}

void VDVariableList::addVarObject(char * shortname, VDVariable * parent, VDVariable * pointer)
{
    int varToken = parser->getToken();
    int addressToken = parser->getToken();

    QString nameString;
    if (parent == NULL)
    {
        nameString = shortname;
    }
    else
    {
        if (parent->type.endsWith("]"))
        {
            nameString = QString("%1[%2]").arg(parent->name).arg(shortname);
        }
        else
        {
            nameString = QString("%1.%2").arg(parent->name).arg(shortname);
        }
    }

    VDVariable * var = new VDVariable(nameString, varToken, addressToken, shortname);
    tokenMap.insert(varToken, var);
    tokenMap.insert(addressToken, var);

    QString prefix;

    if (parent != NULL)
    {
        var->parentPrefix = parent->name;
        var->parent = parent;
        parent->members->append(var);
        var->isNamed = parent->isNamed;
    }

    if (pointer != NULL)
    {
        var->isNamed = false;
        var->isPointer = true;
        pointer->pointsTo = var;
        var->pointers->append(pointer);
        prefix = "*";
    }
    else
    {
        prefix = "";
    }

    nameMap.insert(var->fullName(), var);

    //QString string1 = QString("%1-var-create - * %2%3\n").arg(varToken).arg(prefix).arg(var->name);
    //process->write(string1.toUtf8().data());

    //QString string2 = QString("%1-var-create - * &%2%3\n").arg(addressToken).arg(prefix).arg(var->name);
    //process->write(string2.toUtf8().data());
    writer->writeVarCreate(varToken, QString("%1%2").arg(prefix).arg(var->name));
    writer->writeVarCreate(addressToken, QString("&%1%2").arg(prefix).arg(var->name));
}

bool VDVariableList::updateVariableAtributes(char * t, char * atribute, char * value)
{
    bool isAtribute = false;

    int token = 0;
    token = atoi(t);
    if (token == 0) return false;

    VDVariable * var;
    if (tokenMap.contains(token))
    {
        var = tokenMap.value(token);
    }
    else
    {
        return false;
    }
    if (var->varObjectToken == token)
    {
        isAtribute = true;
        if (strcmp(atribute, "name") == 0)
        {
            var->setVarObject(value);
        }
        else if (strcmp(atribute, "numchild") == 0)
        {
            var->setNumChildren(value);
        }
        else if (strcmp(atribute, "type") == 0)
        {
            var->setType(value);
        }
        else if (strcmp(atribute, "value") == 0)
        {
            var->setValue(value);
        }
    }
    else if (var->addressToken == token)
    {
        isAtribute = true;
        if (strcmp(atribute, "value") == 0)
        {
            if(strcmp(value,"") == 0 || strcmp(value, "0x0") == 0)
            {
                removeVariable(var);
            }
            else
            {
                var->setAddress(value);
            }
        }
    }
    return isAtribute;
}

void VDVariableList::getVariableMembers(VDVariable * var, char * keyword)
{
    int token = parser->getToken();
    var->childrenToken = token;

    tokenMap.insert(token, var);

    QString string;
    if (keyword == NULL)
    {
        //string = QString("%1-var-list-children 0 %2\n").arg(token).arg(var->varobject);
        string = var->varobject;
    }
    else
    {
        string = QString("%1.%2").arg(var->varobject).arg(keyword);
    }
    //process->write(string.toUtf8().data());
    writer->writeVarListChildren(token, string);
}

void VDVariableList::getVariablePoint(VDVariable * var)
{
    //QString pointName = QString("*%1").arg(var->name);
    //addVarObject(pointName.toUtf8().data(), NULL, var);
    addVarObject(var->name.toUtf8().data(), NULL, var);
}

void VDVariableList::resolveExistingAddress(VDVariable * var)
{
    VDVariable * existing = addressMap.value(var->address);

    if (existing == var) return;

    if (existing->name == var->parentPrefix) return;

    if (var->isNamed && !existing->isNamed) //not named = not needed
    {
        nameMap.remove(existing->fullName());
        existing->name = var->name;
        existing->isNamed = true;
    }
    //if (existing->pointsTo == NULL)
    //{
    //    existing->pointsTo = var->pointsTo;
    //}
    while(!var->pointers->isEmpty())
    {
        existing->pointers->append(var->pointers->takeFirst());
    }
    bool isEmpty = ((var->pointers)->isEmpty());
    while(!isEmpty)
    {
        VDVariable * pointer = (var->pointers)->takeFirst();
        if (pointer->pointsTo == var) pointer->pointsTo = existing;
    }
    removeVariable(var);
}

void VDVariableList::resolve(VDVariable * var)
{
    if (!var->address.isEmpty())
    {
        if (!addressMap.isEmpty() && addressMap.contains(var->address))
        {
           resolveExistingAddress(var);
        }
        else
        {
            addressMap.insert(var->address, var);

            if(var->type.endsWith("*"))
            {
                if(!var->address.isEmpty())
                {
                    getVariablePoint(var);
                }
            }
            else
            {
                if(var->numChildren > 0)
                {
                    getVariableMembers(var, NULL);
                }
            }
        }
    }
}

void VDVariableList::listValues()
{
    if(!localsParsed) return;
    if(!tokenMap.isEmpty()) return;

    /*
    QMessageBox msgBox;

    QString str;
    //foreach (VDVariable * var, addressMap)
    foreach (VDVariable * var, nameMap)
    {
        QString varString = QString("%1 %2 = %3 [%4]\n").arg(var->type).arg(var->fullName()).arg(var->value).arg(var->address);
        str += varString;
    }
    str+="\n";
    QMapIterator<QString, VDVariable*> i(addressMap);
    while (i.hasNext())
    {
        i.next();
        VDVariable * adrVar = i.value();
        QString varString = QString("%1: %2\n").arg(i.key()).arg(adrVar->fullName());
        str += varString;
    }

    msgBox.setText(str);
    msgBox.exec();
    */
    makeGraphFile();
}

void VDVariableList::removeToken(char * t)
{
    int token = 0;
    token = atoi(t);
    if (token != 0)
    {
        VDVariable * var = tokenMap.value(token);
        if (var->addressToken == token || var->varObjectToken == token)
        {
            resolve(var);
        }
        tokenMap.remove(token);
        if (tokenMap.isEmpty())
        {
            listValues();
        }
    }
}

void VDVariableList::removeVariable(VDVariable *var)
{
    if (addressMap.contains(var->address))
    {
        VDVariable * existing = addressMap.value(var->address);
        if (existing == var)
        {
            addressMap.remove(var->address);
        }
    }
    if (nameMap.contains(var->fullName()))
    {
        nameMap.remove(var->fullName());
    }
    delete var;
}

void VDVariableList::makeGraphFile()
{
    QFile file("../graphviztest.gv");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << "digraph {" << endl;
    out << "node [shape=box]";
    QString pointId;
    foreach(VDVariable * var, nameMap)
    {
        QString id = var->graphvizId();
        if (var->parent == NULL)
        {
            QString label = var->graphvizLabel().replace(' ', "&#92; ");
            if (var->members->isEmpty())
            {
                out << QString("%1 [label = \"%2\"]\n").arg(id).arg(label);
            }
            else
            {
                out << QString("%1 [shape=record, label = \"%2\"]\n").arg(id).arg(label);
            }
        }
        if(var->type.endsWith("*") && addressMap.contains(var->value))
        {
            VDVariable * point = addressMap.value(var->value);
            if (point != NULL)
            {
                pointId = point->graphvizId();
                out << QString("%1 -> %2\n").arg(id).arg(pointId);
            }
        }
    }
    out << "}";
    file.close();
    QString gvprogram = "dot -Tsvg -o graphviztest.svg graphviztest.gv";
    QProcess * gvprocess = new QProcess();
    gvprocess->setProcessChannelMode(QProcess::MergedChannels);
    gvprocess->setWorkingDirectory("/home/ilze/Documents");
    gvprocess->start(gvprogram);
    gvprocess->waitForFinished();
    gvprocess->close();
    emit window->signalImageParced();
}

void VDVariableList::renderGraphFile()
{
    /*
    GVC_t * gvc;
    Agraph_t * g;
    FILE * fp_gv;
    gvc = gvContext();
    extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
    gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
    fp_gv = fopen("../graphviztest2.gv", "r");
    g = agread(fp_gv, 0);
    gvLayout(gvc, g, "dot");
    FILE * fp_svg;
    //fp_svg = fopen("../graphviztest.svg", "w");
    //gvRender(gvc, g, "svg", fp_svg);
    fp_svg = fopen("../graphviztest.txt", "w");
    gvRender(gvc, g, "plain", fp_svg);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
    */
}

