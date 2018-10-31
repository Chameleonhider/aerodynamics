/* © Algirdas Palubinskas */
/* Fair Use */

#ifndef POINT_MX_CELL_TYPE_HEADER_
#define POINT_MX_CELL_TYPE_HEADER_

#include "point.h"

// houses array of points (within one matrix cell)
typedef struct point_mx_cell_type {
	// point pointer array (=used_stack+main_array)
	uint *point_ind;
	// how many entries in stack "used_stack"
	uint stack_count;
	// maximum number of points
	uint size;
} point_mx_cell_type;

// initialises matrix cell
// pt_mxc - the allocated pointer to initialise
// size - the size of internal stacks
// returns initialised pointer
void pt_mxc_init(point_mx_cell_type *pt_mxc, uint size);
// free()s a matrix cell
// returns NULL
void* pt_mxc_free(point_mx_cell_type *pt_mxc);

// adds a pointer of a point to a matrix cell (init point @ pt_array_point())
// pt_mxc - matrix cell
// point_ind - point index in the point array (which is in the matrix)
uint pt_mxc_point(point_mx_cell_type *pt_mxc, uint point_ind);
// deletes a point from a matrix cell
// pt_mxc - matrix cell
// index - index of point within the matrix cell
void pt_mxc_delete(point_type *point, point_mx_cell_type *pt_mxc, uint index);

// collision between two matrix cells
// pt_mxc_a - matrix cell A
// pt_mxc_b - matrix cell B
void pt_mxc_collision(point_type *point, point_mx_cell_type *pt_mxc_a, point_mx_cell_type *pt_mxc_b, double dt);
// collision inside a matrix cell
// pt_mxc - matrix cell
void pt_mxc_collision_self(point_type *point, point_mx_cell_type *pt_mxc, double dt);

// // NOTE will not be needed, function provided at point_mx.h
// // iterate trough points and check if they should be deleted
// // pt_array - point array to check
// void pt_mxc_lifetime(point_mx_cell_type *pt_mxc);

#endif
