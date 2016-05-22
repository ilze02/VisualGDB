#ifndef VDVARIABLE_H
#define VDVARIABLE_H

#include <QList>
#include <QString>

//class QString;

class VDVariable
{
public:
    QString varobject;
    QString name;
    QString shortname;
    QString type;
    QString value;
    QString address;
    QString parentPrefix;
    QString fullName();
    int varObjectToken;
    int addressToken;
    int evaluateToken;
    int childrenToken;
    int pointToken;
    int numChildren;
    bool isNamed;
    bool isPointer;
    bool numChildrenSet;
    bool addressSet;
    bool typeSet;
    QList<VDVariable*> * members;
    VDVariable * pointsTo;
    QList<VDVariable*> * pointers;
    VDVariable * parent;
public:
    VDVariable();
    ~VDVariable();
    VDVariable(char * n, char * type, char * value, char * address, QList<VDVariable*> * members);
    VDVariable(char * n, int v, int a, char * sn);
    VDVariable(QString n, int v, int a, char * sn);
    void setVarObject(char * o);
    void setName(char * n);
    void setType(char * t);
    void setValue(char * v);
    void setAddress(char * v);
    void setParentPrefix(char * p);
    void setNumChildren(char * c);
    void addMember(VDVariable* member);
    QString graphvizLabel();
    QString graphvizId();
    QString graphvizParentId();
    QString graphvizName();
};

#endif // VDVARIABLE_H
