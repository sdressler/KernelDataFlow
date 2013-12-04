/*
 * AnalyzeOperandValue.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: bzcdress
 */

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <sstream>

vertexIterator BlockProcessor::analyzeOperandValue(llvm::Value *OP) {

    // Check if already analyzed and return
    //vertex v = ValueToVertexMap[OP];
    //if (v > 0) {
    //    return v;
    //}

    std::pair<vertexIterator, bool> v;
    unsigned int ID = OP->getValueID();

    assert(OP && "Error: OP == NULL!");

    if (ID >= llvm::Value::ValueTy::InstructionVal) {

        // Cast to instruction and run instruction analysis
        auto *I = llvm::cast<llvm::Instruction>(OP);
        return analyzeInstruction(I);

    } else if (ID == llvm::Value::ValueTy::ArgumentVal) {

        v = g->addVertex(OP);
        (v.first)->second.name = "ARG";

    } else if (ID == llvm::Value::ValueTy::ConstantIntVal) {

        auto *CI = llvm::cast<llvm::ConstantInt>(OP);

        std::stringstream ss;

        ss << "int_" << CI->getBitWidth() << "b_" << CI->getZExtValue();

        v = g->addVertex(OP);
        (v.first)->second.name = ss.str();

    } else if (ID == llvm::Value::ValueTy::ConstantFPVal) {



        v = g->addVertex(OP);
        (v.first)->second.name = "FP";

    } else {

        llvm::errs()
            << "Operand value not supported: "
            << *OP
            << " ValueID: " << ID << "\n";
        assert(0);

    }

    //ValueToVertexMap[OP] = v;
    (v.first)->second.block = "global";

    return v.first;

}
