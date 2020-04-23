# LLVM-fault-tolerant-dynamic-analysis
This is a project for a simple fault tolerance and isolation project based
on LLVM. The project involves instrumenting an LLVM module in order to
produce a new program with varying policies and strategies for fault
tolerance.

It protect programs from different types of common bugs by checking a few simple policies at runtime. These are all standard bugs that will cause a program to crash, which can cause problems with availability.

1. Heap, global, and stack pointer accesses. Any (1) load from or (2) store to a pointer that occurs in the given IR must access an address that is a valid global, local variable, or heap allocated memory that occurs within the provided IR. Both spatial and temporal safety should be enforced. Once memory is no longer temporally valid, accesses should be invalid. You may assume that all heap memory will be allocated via a call to malloc(). Allocated heap memory will only be freed via a call to free().
2. Invalid frees. Any call to free() must pass a pointer that is either valid or NULL.
3. Divide by zero bugs. Any integer division must have a nonzero denominator.

Fault Policies
==============================================
This project implements a few different policies for handling these types of faults, varying in difficulty. The different policies will allow you to control the quality of service (QOS) of the instrumented program in order to make a trade off between preservation of the original program semantics and availability.

### Policy 1(terminate with warnings)
The simplest way to handle incorrect behaviors is simply to terminate the running program and print (log) an error message describing what form of error has occurred. For this policy, I handle each of the three classes of bugs by simply printing out an error message to stderr and ending the program, making the program return -1 to the system to indicate an error.

You can run it as:

        tolerator <toinstrument.ll> -o instrumented

This should produce a program that can run as:

        ./instrumented <normal> <command> <line> <arguments>

### Policy 2 (Policy 1 + Ignoring invalid side effects)
Consider the impact of behaviors that don't produce a value but rather produce a side effect. In our case, this includes (1) stores to memory and (2) frees of memory. These instructions need not necessarily do anything in order for the program to be well formed. Storing to an invalid location will not even matter if we only read from valid locations. Similarly, no valid memory can be freed when we call free with an invalid address. Thus, we might simply ignore these instructions.

When called with the -ignore argument, the resulting program shall log the bad behavior, as in part 1, but it will ignore the effects of invalid operations without values and continue running.

### Policy 3 (Policy 2 + Default values)
We might take this a step further. Note that we have not been able to avoid reads from invalid memory or division by zero. In these case some value must be produced; we cannot simply ignore the operation. However, we might consider whether the produced value matters a great deal…. If the value does not matter, we could simply return a default value instead.

When is called with the -defaults argument, the resulting program shall log the bad behavior and discard valueless faults, as in part 2, but it will provide erroneous loads from memory and divisions by zero with the value 0 and keep running.

### Policy 4 (Policy 1 + local bypass)
Even more aggressively, we might say that all computation within a function that performs an invalid memory operation or division is suspect, so we really ought to exit the function if we want to avoid the bad behavior while continuing to run. For any function returning void, this is easy, we can simply return directly from the function. For a function that returns a value, we can try returning a zero initialized version of that value type and keep executing.

When your called with the -returns argument, the resulting program shall log the bad behavior and return to the caller of the current function. Similar to task 3, any returned from function that produces a value should produce the zero initalized form of that value.


Building with CMake
==============================================
1. Clone the demo repository.

        git clone http://github.com/nsumner/se-fault-tolerant-template.git

2. Create a new directory for building.

        mkdir tolerantbuild

3. Change into the new directory.

        cd tolerantbuild

4. Run CMake with the path to the LLVM source.

        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=True \
            -DLLVM_DIR=</path/to/LLVM/build>/lib/cmake/llvm/ ../se-fault-tolerant-template

5. Run make inside the build directory:

        make

This produces a tolerator tool called `bin/tolerator` and supporting
libraries in `lib/`.

Note, building with a tool like ninja can be done by adding `-G Ninja` to
the cmake invocation and running ninja instead of make.

Running
==============================================

First suppose that you have a program compiled to bitcode:

    clang -g -c -emit-llvm simpletest.c -o simple.bc

Running `tolerator` with the "log and quit" policy:

    bin/tolerator -log simple.bc -o simple

Running the resulting instrumented program:

    ./simple

