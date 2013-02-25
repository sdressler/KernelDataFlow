/*
 * AnalyzeBB.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: bzcdress
 */

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>

#include <llvm/Support/raw_ostream.h>

void BlockProcessor::analyzeBB(llvm::BasicBlock *BB) {

    for (auto it = BB->begin(); it != BB->end(); ++it) {
        auto v = analyzeInstruction(it);
        v->second.block = BB->getName().str();
    }

    // Create an additional edge to the first instruction of each block
/*
    auto v_inst  = (*g)[BB->begin()];
    auto v_block = (*g)[BB];

    auto e = g->addEdge(v_block, v_inst);
    (*g)[e].ctrl = true;
*/
}
