#include <KernelDataFlow/KernelDataFlow.h>

#include <llvm/Support/raw_ostream.h>

bool KernelDataFlow::runOnModule(Module &M) {

    //Initialize Argument Parser
    //parser = new ArgParser(&g, &M);
    getKernelsFromGlobalAnnotations(
        M.getNamedGlobal(
            "llvm.global.annotations"
        )
    );
    
    errs() << "Found " << KernelFunctions.size() << " Kernels\n";

    for (auto it : KernelFunctions) {

        it->dump();

        // Initialize the Block Processor and run it
        BP = new BlockProcessor(&M, it, &g, &bsg);
        BP->run();

        delete BP;
    }

    errs() << "Done!\n";

    return true;
}

void KernelDataFlow::getKernelsFromGlobalAnnotations(GlobalValue *gv) {

    auto *a = gv->getOperand(0);

    if (a == NULL) {
        errs() << "Something went wrong, going to assert!\n";
        assert(0);
    }

    // Operand 0 Should be a constant array
    if (a->getValueID() != Value::ConstantArrayVal) {
        errs() << "Something went wrong, going to assert!\n";
        assert(0);
    }

    auto *annoStruct = cast<ConstantStruct>(cast<ConstantArray>(a)->getOperand(0));

    if (annoStruct == NULL) {
        errs() << "Something went wrong, going to assert!\n";
        assert(0);
    }

    // Retrieve the name of the annotation, check if it's "kernel"
    auto *aStringVariable = cast<GlobalVariable>(
        cast<ConstantExpr>(annoStruct->getOperand(1))->getOperand(0)
    );

    if (aStringVariable == NULL) {
        errs() << "Something went wrong, going to assert!\n";
        assert(0);
    }

    auto aString = cast<ConstantDataArray>(aStringVariable->getOperand(0))->getAsString();

    // Test for kernel
    if (aString.find("kernel") != std::string::npos) {
        // Proceed
        auto *top = cast<ConstantExpr>(annoStruct->getOperand(0));

        // Test if it's a cast
        if (top->isCast()) {
            KernelFunctions.insert(cast<Function>(top->getOperand(0)));
        } else {
            assert(0);
        }

    } else {
        // Should not happen
        errs() << "Something went wrong, going to assert!\n";
        assert(0);
    }

}

char KernelDataFlow::ID = 0;
static RegisterPass<KernelDataFlow> X(
    "KernelDataFlow",
    "Kernel Data Flow Analysis Pass",
    false,
    false
);
