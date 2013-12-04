#ifndef KERNEL_DATA_FLOW_H
#define KERNEL_DATA_FLOW_H

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>

#include <set>
#include <map>
#include <vector>

#include <KernelDataFlow/BlockProcessor/BlockProcessor.h>

using namespace llvm;

class KernelDataFlow: public ModulePass {
private:
    std::set<Function*> KernelFunctions; /// Holds all kernel functions to be parsed

    G g; /// Holds the constructed graph
    std::map<std::string, G> bsg; /// Holds the per-block subgraphs

    BlockProcessor *BP; /// Holds the Block Processor
public:
	static char ID;
	KernelDataFlow() : ModulePass(ID) {
	    BP = NULL;
	}

/*
	void getAnalysisUsage(AnalysisUsage &AU) const {
		AU.setPreservesCFG();
		AU.addRequired<LoopInfo>();
//        AU.addRequired<TargetData>();
	}
*/

	bool runOnModule(Module &M);

	void getKernelsFromGlobalAnnotations(GlobalValue *gv);
	void basicBlockProcessor(Module &M, Function *F);
/*
	static void writeGraphToFile(const G &g) {

	    std::ofstream f("graph.dot");

	    GraphWriter<G> wr(g);
	    NodePropertyWriter<G> np(g);
	    EdgePropertyWriter<G> ep(g);

	    boost::write_graphviz(f, g, np, ep, wr);

	    f.close();

	}
*/

};

#endif /* KERNEL_DATA_FLOW_H */
