/* © Algirdas Palubinskas */
/* Fair Use */

#ifndef AIRFOIL_NACA4_HEADER_
#define AIRFOIL_NACA4_HEADER_

#include "point.h"
#include "point_mx_cell.h"
#include "point_mx.h"

// Create NACA-4 airfoil, put it into the matrix and a separate point array
// point_mx_type pt_mx - the matrix
// point_count - length of the point array
// scale - how much to scale the airfoil, should be more than hitsize (airfoil length is 1)
// NACA max camber M (single digit, 0 to 10%)
// NACA camber position P (single digit, 0 to 90%)
// NACA thickness T (two digits, 1 to 40%)
// returns pointer to uint array, which is filled with indexes to point array in pt_mx
void* airfoil_naca4_gen(point_mx_type *pt_mx, uint point_count, double scale, double xs, double ys, double M, double P, double T);

#endif
