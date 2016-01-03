/* s2line.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "s2plot.h"
//#include "/Users/too043/docs/astronomy/software/latest/psrchive/Base/Classes/Pulsar/Archive.h"

int main(int argc, char *argv[])
{
   float x[10], y[10], z[10];			/* Arrays of coordinate points */
   int N = 5;					/* Number of points */
   int i;					/* Loop variable */

   srand48((long)time(NULL));			/* Seed random numbers */
   s2opend("/?",argc, argv);			/* Open the display */
   s2swin(-1.,1., -1.,1., -1.,1.);		/* Set the window coordinates */
   //s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);	/* Draw coordinate box */

   for (i=0;i<N;i++) {
      x[i] = drand48()*2.0 - 1.0;		/* Random (x,y,z) coordinates */
      y[i] = drand48()*2.0 - 1.0;
      z[i] = drand48()*2.0 - 1.0;
   }

   s2sci(S2_PG_BLUE);				/* Set colour */
   s2slw(2);					/* Set line width */
   //s2line(N, x, y, z);   			/* Draw the poly-line */
   s2pt(N, x, y, z, 1);   			/* Draw the poly-line */

   s2show(1);					/* Open the s2plot window */
   
   return 1;
}
