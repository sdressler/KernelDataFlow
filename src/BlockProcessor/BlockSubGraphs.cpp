/*
 * BlockSubGraphs.cpp
 *
 *  Created on: Nov 21, 2012
 *      Author: bzcdress
 */

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>

#include <iostream>

G BlockProcessor::generateBlockSubGraphs(std::string block) {

    G sg;

    //std::cerr << "Block: " << block << "\n";

    // First run: add all vertices
    for (auto it = g->vertex_begin(); it != g->vertex_end(); ++it) {

        std::string v_blockName = it->second.block;

        if (v_blockName == block) {
            //std::cerr << "    Vertex: " << it->second.name << "\n";
            sg.addVertex(it);
        }

    }

    // Note: This is distinct to first collect *all* vertices of the block

    // Second run: add all edges
    for (auto it = g->vertex_begin(); it != g->vertex_end(); ++it) {

        std::string v_blockName = it->second.block;

        if (v_blockName == block) {
            for (auto e = g->outEdges(it).first; e != g->outEdges(it).second; ++e) {
                // Check if this edge leads to an external block
                if (g->target(e)->second.block != block) {
                    // Yes, add the target vertex BLOCK prior connecting edges
                    llvm::Value *val = g->target(e)->first;

                    vertexIterator v;

                    if (llvm::BasicBlock::classof(val)) {
                        v = sg.addVertex(val).first;
                        v->second.name = val->getName().str();
                    } else if (llvm::Instruction::classof(val)) {
                        auto *i = llvm::cast<llvm::Instruction>(val);
                        v = sg.addVertex(i->getParent()).first;
                        v->second.name = i->getParent()->getName().str();
                    } else {
                        assert(0);
                    }

                    auto edge = sg.addEdge(g->source(e), v);
                    sg[edge] = (*g)[e];

                } else {
                    auto edge = sg.addEdge(g->source(e), g->target(e));
                    sg[edge] = (*g)[e];
                }

            }
        }

    }

    //Third run: collect global vertices / edges
    for (auto it = g->vertex_begin(); it != g->vertex_end(); ++it) {

        std::string v_blockName = it->second.block;

        if (v_blockName == std::string("global")) {
            // Get outgoing edges and test whether the target belongs to the current local block
            for (auto e = g->outEdges(it).first; e != g->outEdges(it).second; ++e) {
                if (g->target(e)->second.block == block) {
                    // Add vertex
                    sg.addVertex(it);

                    // Connect edges
                    sg.addEdge(g->source(e), g->target(e));
                }
            }
        }

    }

    return sg;

}
