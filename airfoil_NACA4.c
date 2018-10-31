/* © Algirdas Palubinskas */
/* Fair Use */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"
#include "point_mx_cell.h"
#include "point_mx.h"
#include "airfoil_NACA4.h"

void* airfoil_naca4_gen(point_mx_type *pt_mx, uint point_count, double scale, double xs, double ys, double M, double P, double T){
	uint *point_ind = calloc(point_count+(point_count%2), sizeof(uint));
	uint pt_i=0;

	double x;
	double xsq;
	double y;
	double yt;

	double a0=0.2969;
	double a1=-0.126;
	double a2=-0.3516;
	double a3=0.2843;
	double a4=-0.1015; // -0.1015 open, -0.1036 closed

	double i;
	double incr=M_PI*2.0/(point_count-1);
	for (i=0.0; i<M_PI; i=i+incr){
		x = (1-cos(i))*0.5;
		xsq = x*x;

		if (x < P){
			y = M*(2.0*P*x - xsq)/(P*P);
		} else {
			y = M*(1.0 - 2*P + 2*P*x - xsq)/((1.0 - P)*(1.0 - P));
		}
		yt = (T/0.2)*(a0*sqrt(x) + a1*x + a2*xsq + a3*xsq*x + a4*xsq*xsq);

		if (pt_i >= point_count){
			printf("ERR: Tried to fit too many points into the point index array\n");
			printf("pt_i %u point_count %u, i %g incr %g\n", pt_i, point_count, i, incr);
			break;
		}
		point_ind[pt_i] = pt_mx_point(pt_mx, xs-x*scale, (-y+yt)*scale+ys, 0.0, 0.0);
		++pt_i;
		point_ind[pt_i] = pt_mx_point(pt_mx, xs-x*scale, (-y-yt)*scale+ys, 0.0, 0.0);
		++pt_i;
	}

	printf("AIRFOIL_NACA4: Added %i points out of %i\n", pt_i, point_count);
	return point_ind;
}
