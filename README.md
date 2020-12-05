# csci596-finalproject
## Modification of a C version MPI concurrent wavefunction program "mpi_wave.c" to a hybrid MPI+ OpenMP + CUDA code. This program aims to calculate the amplitude along a uniform, vibrating string after a specified amount of time, which is similar to 1D particle in a box wavefuntions problem.
- Calculation decomposition
  1. the amplitude on the y axis
  2. i as the position index along the x axis
  3. node points imposed along the string
  4. update of the amplitude at discrete time steps
- The equation to be solved in the program: 1D wavefunction
   A(i,t+1) = (2.0 * A(i,t)) - A(i,t-1) + (c * (A(i-1,t) - (2.0 * A(i,t)) + A(i+1,t))), where A is the amplitude along the y axis, c is a constant, which can be tuned in the calculation. The amplitude will depend on previous timesteps (t, t-1) and neighboring points (i-1, i+1).

![699px-Particle_in_a_box_wavefunctions svg](https://user-images.githubusercontent.com/72051087/99919070-f8dcbb80-2ccf-11eb-8abf-a1fce3f2422f.png)

-Pieces of work can be performed concurrently
 1. specify
 
-utilize two CPU cores and two GPU devices on each compute node. This is achieved by launching one MPI rank per node, where each rank spawns two OpenMP threads that run on different CPU cores and use different GPU devices.

 
