#ifndef GRAPH_PROPERTIES_H
#define GRAPH_PROPERTIES_H

#include <llvm/BasicBlock.h>

#include <string>
#include <memory>

struct GraphVertexProperties {
    std::string name;
    std::string block;
    //std::shared_ptr<llvm::Value> value;
    bool ctrl = false;
    llvm::Value *value;
};

struct GraphEdgeProperties : public GraphVertexProperties {
    llvm::Type *type;
};

#endif /* GRAPH_PROPERTIES_H */
