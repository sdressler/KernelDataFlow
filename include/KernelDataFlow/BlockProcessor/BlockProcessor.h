/*
 * BlockProcessor.h
 *
 *  Created on: Nov 11, 2012
 *      Author: bzcdress
 */

#ifndef BLOCKPROCESSOR_H_
#define BLOCKPROCESSOR_H_

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

#include <KernelDataFlow/Graph.h>

#include <map>
#include <string>

class BlockProcessor {
private:
    llvm::Module *M;
    llvm::Function *F;

    G *g;
    std::map<std::string, G> *bsg;

public:
    BlockProcessor(llvm::Module *_M, llvm::Function *_F, G *_g, std::map<std::string, G> *_bsg) {
        M = _M;
        F = _F;
        g = _g;
        bsg = _bsg;
    }

    void run();

    void analyzeBB(llvm::BasicBlock *BB);

    vertexIterator analyzeInstruction(llvm::Instruction *I);
    vertexIterator analyzeOperandValue(llvm::Value *OP);

    G generateBlockSubGraphs(std::string block);

};

#endif /* BLOCKPROCESSOR_H_ */
