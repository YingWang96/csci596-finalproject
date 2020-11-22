# csci596-finalproject
## Modification of a C version MPI concurrent wavefunction program "mpi_wave.c" to a hybrid MPI+ OpenMP + CUDA code. This program aims to calculate the amplitude along a uniform, vibrating string after a specified amount of time
- Calculation decomposition
  1. the amplitude on the y axis
  2. i as the position index along the x axis
  3. node points imposed along the string
  4. update of the amplitude at discrete time steps
- The equation to be solved in the program: 1D wavefunction
   A(i,t+1) = (2.0 * A(i,t)) - A(i,t-1) + (c * (A(i-1,t) - (2.0 * A(i,t)) + A(i+1,t))), where A is the amplitude along the y axis, c is a constant, which can be tuned in the calculation
