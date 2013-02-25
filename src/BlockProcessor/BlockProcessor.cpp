/*
 * BlockProcessor.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: bzcdress
 */

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>
#include <KernelDataFlow/KernelDataFlow.h>

#include <llvm/Support/raw_ostream.h>

void BlockProcessor::run() {

    llvm::errs() << "BB processor attached.\n";

    // First pass create all necessary "root" vertices
    for (auto it = F->begin(); it != F->end(); ++it) {

        // Insert new vertex into the graph and also register it
        // in the vertex / name map
        std::string BBName = it->getName().str();

        auto v_BB = g->addVertex(it);
        (v_BB.first)->second.name = BBName;

    }

    // Second pass create all sub-vertices
    for (auto it = F->begin(); it != F->end(); ++it) {
        // Dive into the BB and create necessary edges
        analyzeBB(it);
    }

    MicroGraphWriter<G> graphWriter("graph.dot", *g);
    graphWriter.writeGraphToFile();

    // Generate block-centric subgraphs
    for (auto it = F->begin(); it != F->end(); ++it) {
        std::string BBName = it->getName().str();

        auto _g = generateBlockSubGraphs(it->getName().str());

        bsg->insert(std::make_pair(BBName, _g));
    }

    //llvm::errs() << blockSubGraphs.size() << "\n";
    for (auto it = bsg->begin(); it != bsg->end(); ++it) {
        MicroGraphWriter<G> graphWriter(
            std::string("graphs/graph_") + it->first + std::string(".dot"),
            it->second
        );
        graphWriter.writeGraphToFile();
    }

}
