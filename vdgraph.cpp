#include "vdgraph.h"
#include <gvc.h>
#include <QString>
#include <QtGlobal>
#include <QMap>
#include <QPair>

/// The agopen method for opening a graph
static inline Agraph_t* _agopen(QString name, Agdesc_t kind)
{
    return agopen(const_cast<char *>(qPrintable(name)), kind, NULL);
}

/// Add an alternative value parameter to the method for getting an object's attribute
static inline QString _agget(void *object, QString attr, QString alt=QString())
{
    QString str=agget(object, const_cast<char *>(qPrintable(attr)));

    if(str==QString())
        return alt;
    else
        return str;
}

/// Directly use agsafeset which always works, contrarily to agset
static inline int _agset(void *object, QString attr, QString value)
{
    return agsafeset(object, const_cast<char *>(qPrintable(attr)),
                     const_cast<char *>(qPrintable(value)),
                     const_cast<char *>(qPrintable(value)));
}

VDGraph::VDGraph()// : _context(gvContext()), _graph(_agopen("G", Agdirected))
 // Strict directed graph, see libgraph doc
{
    _graph = _agopen("G", Agdirected);
    _context = gvContext();

    //Set graph attributes
    //_agset(_graph, "overlap", "prism");
    //_agset(_graph, "splines", "true");
    //_agset(_graph, "pad", "0,2");
    //_agset(_graph, "dpi", "96,0");
    //_agset(_graph, "nodesep", "0,4");

    //Set default attributes for the future nodes
    //_agnodeattr(_graph, "fixedsize", "true");
    //_agnodeattr(_graph, "label", "");
    //_agnodeattr(_graph, "regular", "true");

    //Divide the wanted width by the DPI to get the value in points
    //QString nodePtsWidth("%1").arg("14"/_agget(_graph, "dpi", "96,0").toDouble());
    //GV uses , instead of . for the separator in floats
    //_agnodeattr(_graph, "width", nodePtsWidth.replace('.', ","));

    //setFont("Arial");
}

VDGraph::~VDGraph()
{
    gvFreeLayout(_context, _graph);
    agclose(_graph);
    gvFreeContext(_context);
}

