KernelDataFlow
==============

This is an LLVM pass to traverse a "kernel function" (KF) and record its data
flow graph. The resulting graph is constructed such that it does not only
provides data flows but also a bit of control flow.

Dependencies
------------

* LLVM >= 3.2
* Clang is preferred
* make

Limitations
-----------

It is very likely, that this pass does not handle every input. Thus, asserts may
happen on various situations. The tool is easily extensible. To add a missing
instruction one has to extend `src/BlockProcessor/AnalyzeInstruction.cpp`.i

Installation
------------

1. Match the settings in `Makefile.in` to your needs
2. Type `make`
3. Use the generated `KernelDataVolume.so` as LLVM pass

Usage
-----

You can input C or C++ code where the kernel function is annotated. Annotation
is done by setting `__attribute__((annotate("kernel")))` right before the kernel
function, i.e. its declaration or definition. Once the kernel function was
processed, a graph is generated which currently is written to disk as Graphviz
file. This file can then be further processed, e.g. to create a visualization,
for example:

`$ clang -emit-llvm -S -c example/loop.c -o - | opt -load ./KernelDataFlow.so -KernelDataFlow && dot -Tpdf graph.dot -o graph.pdf`

Furthermore, block-related graphs can be found within the `graphs` directory.
The pass is yet extendable to also use the in-memory graph. To do so, simply
create a new folder inside the `src` directory, copy the Makefile of one of
the other sub-directories and adapt it. Also adapt the Makefiles in `src` and
in the root directory as well.
