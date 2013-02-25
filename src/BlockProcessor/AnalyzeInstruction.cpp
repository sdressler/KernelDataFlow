/*
 * AnalyzeInstruction.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: bzcdress
 */

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>

#include <llvm/Support/raw_ostream.h>

vertexIterator BlockProcessor::analyzeInstruction(llvm::Instruction *I) {

    // Already analyzed?
    auto v_test = (*g)[I];
    if (v_test != g->vertex_end()) {
        return v_test;
    }

    auto v = g->addVertex(I);

    if (llvm::AllocaInst::classof(I)) {

        (v.first)->second.name = "ALLOC";

    } else if (llvm::GetElementPtrInst::classof(I)) {

        (v.first)->second.name = "GEP_" + I->getName().str();

        auto *GEP = llvm::cast<llvm::GetElementPtrInst>(I);

        // Get vertex for used element and add edge
        auto v_src = analyzeOperandValue(GEP->getPointerOperand());
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = GEP->getPointerOperand()->getType();

        // Get indices, assign values and edges
        for (auto it = GEP->idx_begin(); it != GEP->idx_end(); ++it) {
            auto v_idx = analyzeOperandValue(*it);
            auto e = g->addEdge(v_idx, v.first);
            (*g)[e].type = &(*it->get()->getType());
        }

    } else if (llvm::LoadInst::classof(I)) {

        (v.first)->second.name = "LOAD";

        auto *LI = llvm::cast<llvm::LoadInst>(I);

        // Analyze pointer and add edge
        auto v_src = analyzeOperandValue(LI->getPointerOperand());
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = LI->getPointerOperand()->getType()->getContainedType(0);

    } else if (llvm::StoreInst::classof(I)) {

        (v.first)->second.name = "STORE";

        auto *SI = llvm::cast<llvm::StoreInst>(I);

        auto v_src = analyzeOperandValue(SI->getPointerOperand());
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = SI->getPointerOperand()->getType()->getContainedType(0);

    } else if (llvm::CmpInst::classof(I)) {

        (v.first)->second.name = "CMP";

        auto *CI = llvm::cast<llvm::CmpInst>(I);

        // Analyze *both* operands and add edges
        for (unsigned int i = 0; i < 2; i++) {
            auto v_op = analyzeOperandValue(CI->getOperand(i));
            auto e = g->addEdge(v_op, v.first);
            (*g)[e].type = CI->getOperand(i)->getType();
        }

    } else if (llvm::BranchInst::classof(I)) {

        (v.first)->second.name = "BR";

        auto *BI = llvm::cast<llvm::BranchInst>(I);

        if (BI->isConditional()) {

            // Get compare as input, add blocks as output
            auto v_cmp = analyzeOperandValue(BI->getCondition());
            g->addEdge(v_cmp, v.first);

            for (unsigned int i = 0; i < 2; i++) {
                auto *BB = BI->getSuccessor(i);

                //auto v_block = g->get_vertex(BB);
                auto v_block = (*g)[(BB)];
                auto e = g->addEdge(v.first, v_block);

                std::string str_cond;
                if (i == 0) {
                    str_cond = "T";
                } else {
                    str_cond = "F";
                }

                (*g)[e].name = str_cond;
/*
                (*g)[e.first].name = str_cond;
                (*g)[e.first].value = BB;
                (*g)[e.first].ctrl = true;
*/
            }

        } else {

            // Assign output block
            //vertex v_block = g->get_vertex(BI->getSuccessor(0));
            auto v_block = (*g)[(BI->getSuccessor(0))];
            g->addEdge(v.first, v_block);
            //(*g)[e.first].ctrl = true;

            // Get the previous instruction, if available and assign edge
            auto *BB = BI->getParent();

            // Test if this instruction is already the first
            if (BI == BB->begin()) {
                // This is the case, so connect "this" block as input edge
                //v_block = g->get_vertex(BB);
                v_block = (*g)[(BB)];
                g->addEdge(v_block, v.first);
            } else {
                // Not the case, analyze previous instruction
                auto v_prev = analyzeInstruction(BI->getPrevNode());
                g->addEdge(v_prev, v.first);
            }
            //(*g)[e.first].ctrl = true;

        }

    } else if (llvm::PHINode::classof(I)) {

        (v.first)->second.name = "PHI";

        auto *PHI = llvm::cast<llvm::PHINode>(I);

        for (auto it = PHI->block_begin(); it != PHI->block_end(); ++it) {

            // Get the block vertex
            auto v_block = (*g)[(*it)];

            // Get the value vertex
            auto v_value = analyzeOperandValue(
                PHI->getIncomingValueForBlock(*it)
            );

            // Add edges
            auto e = g->addEdge(v_block, v.first);
            (*g)[e].value = v_value->first;
            (*g)[e].name = v_value->second.name;

            // Remove temporary value vertex
            g->removeVertex(v_value);

        }

        // Next instruction
        auto v_next = analyzeInstruction(PHI->getNextNode());
        g->addEdge(v.first, v_next);
        //(*g)[e.first].ctrl = true;

    } else if (llvm::BinaryOperator::classof(I)) {

        (v.first)->second.name = "BOP";

        auto *BOP = llvm::cast<llvm::BinaryOperator>(I);

        // Analyze *both* operands and add edges
        for (unsigned int i = 0; i < 2; i++) {
            auto v_op = analyzeOperandValue(BOP->getOperand(i));
            auto e = g->addEdge(v_op, v.first);
            (*g)[e].type = BOP->getOperand(i)->getType();
        }

    } else if (llvm::ZExtInst::classof(I)) {

        (v.first)->second.name = "ZEXT";

        auto *ZE = llvm::cast<llvm::ZExtInst>(I);

        auto v_src = analyzeOperandValue(ZE->getOperand(0));
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = ZE->getOperand(0)->getType();

    } else if (llvm::SelectInst::classof(I)) {

        (v.first)->second.name = "SELECT";

        auto *SI = llvm::cast<llvm::SelectInst>(I);

        // Get source compare vertex
        auto v_cmp = analyzeOperandValue(SI->getCondition());
        g->addEdge(v_cmp, v.first);

    } else if (llvm::SExtInst::classof(I)) {

        (v.first)->second.name = "SEXT";

        auto *SE = llvm::cast<llvm::SExtInst>(I);

        auto v_src = analyzeOperandValue(SE->getOperand(0));
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = SE->getOperand(0)->getType();

    } else if (llvm::CallInst::classof(I)) {

        llvm::errs() << "Call not supported!\n";
        assert(0);

    } else if (llvm::ReturnInst::classof(I)) {

        (v.first)->second.name = "RET";

        auto *RET = llvm::cast<llvm::ReturnInst>(I);

        // Get return value vertex
        auto retval = RET->getReturnValue();

        if (retval != NULL) {
            auto v_ret = analyzeOperandValue(retval);
            g->addEdge(v.first, v_ret);
        }

    } else if (llvm::BitCastInst::classof(I)) {

        (v.first)->second.name = "BITCAST";

        auto *BC = llvm::cast<llvm::BitCastInst>(I);

        auto v_src = analyzeOperandValue(BC->getOperand(0));
        auto e = g->addEdge(v_src, v.first);
        (*g)[e].type = BC->getOperand(0)->getType();

    } else {

        llvm::errs() << "Instruction not handled: " << *I << "\n";
        assert(0);

    }

    return v.first;
}

