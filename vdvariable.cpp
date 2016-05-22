#include <QList>
#include <QString>
#include "vdvariable.h"

VDVariable::VDVariable()
{
}

VDVariable::VDVariable(char * n, char * t, char * v, char * a, QList<VDVariable*> * m)
{
    name = QString(n);
    type = QString(t);
    value = QString(v);
    address = QString(a);
    members = m;
    isNamed = true;
    isPointer = false;
    //numChildrenSet = false;
    //addressSet = false;
    //typeSet = false;
    //pointers = new QList<VDVariable*>;
    members = new QList<VDVariable*>;
    parent = NULL;
    //pointsTo = NULL;
    shortname = QString(n);
}

VDVariable::VDVariable(char * n, int v, int a, char * sn)
{
    name = QString::fromLatin1(n, -1);
    varObjectToken = v;
    addressToken = a;
    numChildren = 0;
    isNamed = true;
    isPointer = false;
    //numChildrenSet = false;
    //addressSet = false;
    //typeSet = false;
    //pointers = new QList<VDVariable*>;
    members = new QList<VDVariable*>;
    parent = NULL;
    //pointsTo = NULL;
    shortname = QString(sn);
}

VDVariable::VDVariable(QString n, int v, int a, char * sn)
{
    name = n;
    varObjectToken = v;
    addressToken = a;
    numChildren = 0;
    isNamed = true;
    isPointer = false;
    //numChildrenSet = false;
    //addressSet = false;
    //typeSet = false;
    //pointers = new QList<VDVariable*>;
    members = new QList<VDVariable*>;
    parent = NULL;
    //pointsTo = NULL;
    shortname = QString(sn);
}

VDVariable::~VDVariable()
{
    //delete pointers;
    delete members;
}

void VDVariable::addMember(VDVariable* member)
{
    members->append(member);
}

void VDVariable::setVarObject(char * o)
{
    varobject = QString(o);
}

void VDVariable::setName(char * n)
{
    name = QString(n);
}

void VDVariable::setType(char * t)
{
    type = QString(t);
}

void VDVariable::setValue(char * v)
{
    value = QString(v);
}

void VDVariable::setAddress(char * a)
{
    address = QString(a);
}

void VDVariable::setNumChildren(char * c)
{
    numChildren = atoi(c);
}

QString VDVariable::fullName()
{
    QString pointerSuffix;
    if (isPointer)
    {
        pointerSuffix = "->";
    }
    else
    {
        pointerSuffix = "";
    }
    if (parent == NULL)
    {
        return QString("%1%2").arg(shortname).arg(pointerSuffix);
    }
    else
    {
        return QString("%1.%2%3").arg(parent->fullName()).arg(shortname).arg(pointerSuffix);
    }
}

QString VDVariable::graphvizLabel()
{
    if (type.endsWith("*"))
    {
        return QString("%1 %2").arg(type).arg(graphvizName());
    }
    else
    {
        if (members->isEmpty())
        {
            return QString("%1 %2 = %3").arg(type).arg(graphvizName()).arg(value);
        }
        else
        {
            QString label = QString("{%1 %2").arg(type).arg(graphvizName());
            foreach(VDVariable * var, * members)
            {
                label += QString("|<%1>%2").arg(var->varobject).arg(var->graphvizLabel());
            }
            label += "}";
            return label;
        }
    }
}

QString VDVariable::graphvizId()
{
    if (parent == NULL)
    {
        return QString(varobject);
    }
    else
    {
        return QString("%1:%2").arg(parent->graphvizParentId()).arg(varobject);
    }
}

QString VDVariable::graphvizParentId()
{
    if (parent == NULL)
    {
        return graphvizId();
    }
    else
    {
        return parent->graphvizParentId();
    }
}

QString VDVariable::graphvizName()
{
    if(isNamed || parent != NULL) return name;
    else return "";
}

