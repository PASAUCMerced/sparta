Sparta
-----------

Sparta is a high-performance, element-wise sparse tensor contraction software on multicore CPU with heterogeneous memory. The sparse tensor contraction is critical to the overall performance of scientific applications, such as quantum chemistry and quantum physics. Sparta is implemented based on the open-sourced Hierarchical Parallel Tensor Infrastructure (HiParTI) library (https://gitlab.com/tensorworld/hiparti), which supports fast essential sparse tensor operations with matrices and vectors and tensor decompositions on multicore CPU and GPU architectures.

# Getting Started Guide:

## Build requirements:

- C Compiler (>=v7.5)

- [CMake](https://cmake.org) (>v3.0)

- [OpenBLAS](http://www.openblas.net)

- [NUMA](https://linux.die.net/man/3/numa)


## Build:
1. `cd sparta` & `./build.sh`

## Parameters:
You can check the parameters options with `./sparta/build/ttt --help`
```
Options: -X FIRST INPUT TENSOR
         -Y FIRST INPUT TENSOR
         -Z OUTPUT TENSOR (Optinal)
         -m NUMBER OF CONTRACT MODES
         -x CONTRACT MODES FOR TENSOR X (0-based)
         -y CONTRACT MODES FOR TENSOR Y (0-based)
         -t NTHREADS, --nt=NT (Optinal)
         --help
```

## Contributiors

* Jiawen Liu (Contact: jliu265@ucmerced.edu)
* Dong Li    (Contact: dli35@ucmerced.edu)
* Jiajia Li  (Contact: Jiajia.Li@pnnl.gov)
