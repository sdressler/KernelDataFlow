#ifndef GRAPH_H
#define GRAPH_H

#include <MicroGraph/MicroGraph.h>
#include <KernelDataFlow/GraphProperties.h>

typedef MicroGraph<llvm::Value*, GraphVertexProperties, GraphEdgeProperties> G;

typedef G::tVertexIterator vertexIterator;
typedef G::tEdgeIterator edgeIterator;

#endif /* GRAPH_H */
