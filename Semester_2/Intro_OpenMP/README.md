# Introduction to OpenMP

Requirements:

- Clang
- OpenMP

To compile and generate file run (on MacOS):

    clang -Xclang -fopenmp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp hello.c -o hello

