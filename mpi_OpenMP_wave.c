/***************************************************************************
 *  * FILE: mpi_wave.c
 *  * DESCRIPTION:
 *  *   MPI Concurrent Wave Equation - C Version
 *  *   Point-to-Point Communications Example
 *  *   This program implements the concurrent wave equation described 
 *  *   in Chapter 5 of Fox et al., 1988, Solving Problems on Concurrent
 *  *   Processors, vol 1.  
 *  *   A vibrating string is decomposed into points.  Each processor is 
 *  *   responsible for updating the amplitude of a number of points over
 *  *   time. At each iteration, each processor exchanges boundary points with
 *  *   nearest neighbors.  This version uses low level sends and receives
 *  *   to exchange boundary points.
 *  *  AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *  *    Center. Converted to MPI: George L. Gusciora, MHPCC (1/95)  
 *  * LAST REVISED: 07/05/05
 *  * modified by Ying Wang (from MPI to hybrid MPI + OpenMP) 12/06/2020
 *                 ***************************************************************************/
#include <mpi.h>
#include <omp.h> /*including OpenMP*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MASTER 0
#define TPOINTS 800
#define MAXSTEPS  10000
#define PI 3.14159265
#define MAX_THREADS 8

int RtoL = 10;  /*matching message tags*/
int LtoR = 20;
int OUT1 = 30;
int OUT2 = 40;

void init_master(void);/*Master obtains timestep input value from user and broadcasts it*/
void init_workers(void);/*Workers receive timestep input value from master*/
void init_line(int npts, int tid, int width, int nthreads, int taskid); /*All processes initialize points on line*/
void update (int left, int right);/*All processes update their points a specified number of times*/ 
void output_master(void);/*Master receives results from workers and prints*/
void output_workers(void);/*Workers send the updated values to the master*/
//extern void draw_wave(double *);/* display results with draw_wave routine */ 

int	taskid,               /* task ID */
	numtasks,             /* number of processes */
	nsteps,               /* number of time steps */
	npoints,              /* number of points handled by this processor */
	first;                /* index of 1st point handled by this processor */
double	etime,                /* elapsed time in seconds */
	values[TPOINTS+2],  /* values at time t */
	oldval[TPOINTS+2],  /* values at time (t-dt) */
	newval[TPOINTS+2];  /* values at time (t+dt) */

/*  ------------------------------------------------------------------------
 *   *  Master obtains timestep input value from user and broadcasts it
 *    *  ------------------------------------------------------------------------ */
void init_master(void) {
   char tchar[8];


   /* Set number of number of time steps and then print and broadcast*/
   nsteps = 0;
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps (1-%d): \n",MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Enter value between 1 and %d\n", MAXSTEPS);
      }
   MPI_Bcast(&nsteps, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
   printf("master part finished");
   }

/*  -------------------------------------------------------------------------
 *   *  Workers receive timestep input value from master
 *    *  -------------------------------------------------------------------------*/
void init_workers(void) {
   MPI_Bcast(&nsteps, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
   printf("workers part finished");
   }

/*  ------------------------------------------------------------------------
 *   *  All processes initialize points on line
 *    *  --------------------------------------------------------------------- */
void init_line(int npts, int tid, int width, int nthreads, int taskid) {
   int i, j, k;
   double x, fac;

   /* calculate initial values based on sine curve */
   fac = 2.0 * PI;
   printf("fac= %e",fac);
   for (i = 0, k = 0; i < numtasks; i++) {
      
      if (taskid == i) {
         first = k + 1 + tid*width;
         printf("first: %d\n",first);
         k = tid*width;
         printf("k= %d\n",k);
         npoints = npts;
         printf ("task=%3d tid=%3d first point=%5d  npoints=%4d\n", taskid, tid, 
                 first, npts);
         
         for (j = 1+tid*width; j <= nthreads*width; j++, k++) {
            x = (double)k/(double)(TPOINTS - 1);
//            printf("x=%e\n",x);
            values[j] = sin (fac * x);
//            printf("value=%e\n",values[j]);
         } 
      }
      else  k += npts;
//   printf("initialization finished!");
   for (i = 1; i <= npoints; i++) 
     oldval[i] = values[i];
   printf("This is a test\n");
   }
}

/*  -------------------------------------------------------------------------
 *   *  All processes update their points a specified number of times 
 *    *  -------------------------------------------------------------------------*/
void update(int left, int right) {
   int i, j;
   double dtime, c, dx, tau, sqtau;
   MPI_Status status;

   dtime = 0.3;  
   c = 1.0;
   dx = 1.0;
   tau = (c * dtime / dx);
   sqtau = tau * tau;

   /* Update values for each point along string */
   for (i = 1; i <= nsteps; i++) {
      /* Exchange data with "left-hand" neighbor */
      if (first != 1) {
         MPI_Send(&values[1], 1, MPI_DOUBLE, left, RtoL, MPI_COMM_WORLD); /*send left endpoint to left neighbor*/
         MPI_Recv(&values[0], 1, MPI_DOUBLE, left, LtoR, MPI_COMM_WORLD,
                  &status); /*receive left endpoint from right neighbor*/
         }
      /* Exchange data with "right-hand" neighbor */
      if (first + npoints -1 != TPOINTS) {
         MPI_Send(&values[npoints], 1, MPI_DOUBLE, right, LtoR, MPI_COMM_WORLD); /*send right endpoint to right neighbor*/
         MPI_Recv(&values[npoints+1], 1, MPI_DOUBLE, right, RtoL,
                   MPI_COMM_WORLD, &status); /* receive right endpoint from left neighbor */
         }
      /* Update points along line */
      for (j = 1; j <= npoints; j++) {
         /* Global endpoints */
         if ((first + j - 1 == 1) || (first + j - 1 == TPOINTS))
            newval[j] = 0.0;
         else
            /* Use wave equation to update points */
            newval[j] = (2.0 * values[j]) - oldval[j]
               + (sqtau * (values[j-1] - (2.0 * values[j]) + values[j+1]));
         }
      for (j = 1; j <= npoints; j++) {
         oldval[j] = values[j];
         values[j] = newval[j];
         }
      }
   }

/*  ------------------------------------------------------------------------
 *   *  Master receives results from workers and prints
 *    *  ------------------------------------------------------------------------ */
void output_master(void) {
   int i,j, start, npts, buffer[2];
   double results[TPOINTS];
   FILE *fp;
   MPI_Status status;
 
   /* Store worker's results in results array */
   for (i = 1; i < numtasks; i++) {
      /* Receive first point, number of points and results */
      MPI_Recv(buffer, 2, MPI_INT, i, OUT1, MPI_COMM_WORLD, &status);
      start = buffer[0];
      npts = buffer[1];
      MPI_Recv(&results[start-1], npts, MPI_DOUBLE, i, OUT2,
               MPI_COMM_WORLD, &status);
      }

   /* Store master's results in results array */
   for (i = first; i < first + npoints; i++)
      results[i-1] = values[i];

   j = 0;
   printf("***************************************************************\n");
   printf("Final amplitude values for all points after %d steps:\n",nsteps);
   for (i = 0; i < TPOINTS; i++) {
      printf("%d %6.2f\n ", i,results[i]);
/*      j = j++;
      if (j == 10) {
         printf("\n");
         j = 0;
         }*/
      }
   fp = fopen("mpi_results.dat", "w+");
   for (i = 0; i < TPOINTS; i++) {
      fprintf(fp,"%d %6.2f\n ", i,results[i]);
   }
   fclose(fp);
   printf("***************************************************************\n");
   /*printf("\nDrawing graph...\n");
   printf("Click the EXIT button or use CTRL-C to quit\n");

    display results with draw_wave routine
   draw_wave(&results[0]);*/
   }

/*  -------------------------------------------------------------------------
 *   *  Workers send the updated values to the master
 *    *  -------------------------------------------------------------------------*/
 
void output_workers(void) {
   int buffer[2];
   MPI_Status status;

   /* Send first point, number of points and results to master */
   buffer[0] = first;
   buffer[1] = npoints;
   MPI_Send(&buffer, 2, MPI_INT, MASTER, OUT1, MPI_COMM_WORLD);
   MPI_Send(&values[1], npoints, MPI_DOUBLE, MASTER, OUT2, MPI_COMM_WORLD);
   }

/*  ------------------------------------------------------------------------
 *   *  Main program
 *    *  ------------------------------------------------------------------------ */

int main (int argc, char *argv[])
{
//  shared variables among all thread
int left, right, rc;
int nmin, nleft, npts, nthreads, tid, width;
double pi, pig;
/* Initialize MPI */
MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
if (numtasks < 2) {
  printf("ERROR: Number of MPI tasks set to %d\n",numtasks);
  printf("Need at least 2 tasks!  Quitting...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(0);
}

if (taskid == numtasks-1)
   right = 0;
else
   right = taskid + 1;
if (taskid == 0)
   left = numtasks - 1;
else
   left = taskid - 1;

if (taskid == MASTER) {
   printf ("Starting mpi_wave using %d tasks.\n", numtasks);
   printf ("Using %d points on the vibrating string.\n", TPOINTS);
   init_master();
   }
else
   init_workers();

omp_set_num_threads(8);
#pragma omp parallel private(tid) // tid: figure out the meaning and the function of tid
{
  int i;
  nthreads = omp_get_num_threads();
  tid = omp_get_thread_num();
  
  printf("numtasks: %d\n", numtasks);
  nmin = TPOINTS/numtasks;
  printf("nmin: %d\n", nmin);
  nleft = TPOINTS%numtasks;
  printf("nleft: %d\n", nleft);
  npts = (numtasks <= nleft) ? nmin + 1 : nmin;
  printf("npts: %d\n", npts);
  width = npts/nthreads;
  printf("width: %d", width);

  init_line(npts, tid, width, nthreads, taskid);
//    update(left, right);
//  printf("initialization is done!");
}	// the part needs to be refined

printf("initialization is done!");

/*init_line();*/

/* Update values along the line for nstep time steps*/
update(left, right);

/* Master collects results from workers and prints */
if (taskid == MASTER)
   output_master();
else
   output_workers();

MPI_Allreduce(&pi,&pig,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD); // Inter-thread reduction Inter-rank reduction
MPI_Finalize();
return 0;
}
