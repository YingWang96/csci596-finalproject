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

###This is an example of a problem involving data dependencies. A parallel solution will involve communications and synchronization.The entire amplitude array is partitioned and distributed as subarrays to all tasks. Each task owns an equal portion of the total array.

- Steps to construct a parallel program
  1. Identify what pieces of work can be performed concurrently. 
  2. Partition concurrent work onto independent processors. 
  3. Distribute a program’s input, output, and intermediate data
  4. Coordinate accesses to shared data: avoid conflicts: Ensure proper order of work using synchronization

 
-utilizing two nodes and eight threads on each node. This is achieved by launching one MPI rank per node, where each rank spawns eight OpenMP threads that run on different CPU cores and use different GPU devices.

##Result
- The following figure shows the results of running the code mpi_wave.c 100, 500, 1000, 5000, 10000 timesteps.
![mpi-results](https://user-images.githubusercontent.com/72051087/101733957-77797d00-3a74-11eb-983e-c8b148f09feb.png)
- The problem with the mpi_OpenMP.c code. Finally, I can't make it output the correct result. This code has the problem with initialize the amplitude along the line. I also attached the output of mpi_OpenMP.c file. It shows that it stucks in proceeding by the second rank.
- Due to the incompleteness of moi_OpenMP.c, I can't refine the original mp



 
