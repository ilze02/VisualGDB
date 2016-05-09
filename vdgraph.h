#ifndef VDGRAPH_H
#define VDGRAPH_H
#include <QMap>

class QString;
class QReal;
class QString;
typedef struct Agraph_s Agraph_t;
typedef struct Agnode_s Agnode_t;
typedef struct Agedge_s Agedge_t;
typedef struct GVC_s GVC_t;

class VDGraph
{
public:
    VDGraph();
    ~VDGraph();
public:
    /// Add and remove nodes
    void addNode(const QString& name);
    void removeNode(const QString& name);
    void clearNodes();

    /// Add and remove edges
    void addEdge(const QString& source, const QString& target);
    void removeEdge(const QString& source, const QString& target);

private:
    GVC_t *_context;
    Agraph_t *_graph;
    QMap<QString, Agnode_t*> _nodes;
    QMap<QPair<QString, QString>, Agedge_t*> _edges;
};

#endif // VDGRAPH_H
