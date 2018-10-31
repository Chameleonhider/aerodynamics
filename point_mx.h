/* © Algirdas Palubinskas */
/* Fair Use */

#ifndef POINT_MX_TYPE_HEADER_
#define POINT_MX_TYPE_HEADER_

#include "point.h"
#include "point_mx_cell.h"
#include "point_mx.h"

// the whole matrix, spanning the entire screen
typedef struct point_mx_type {
	// main point array
	point_type *point;
	// used stack contains "point" indexes in use
	uint *used_stack;
	// free stack contains "point" indexes not in use (filled in reverse)
	uint *free_stack;
	// how many entries in stack "used_stack"
	uint stack_count;
	// maximum total number of points
	uint size;

	// default velocity points are trying to maintain, especially when close to boundary
	double defvx;
	double defvy;
	// maximum iteration of life for point
	uint life;

	// memory structure:
	//0 3 6
	//1 4 7
	//2 5 8
	// iteration: for(x){ for(y){ ... } } to optimise caching
	point_mx_cell_type *cell_main;
	point_mx_cell_type **cell_x;

	// number of bucket in x axis (sizex)
	uint mxc_countx;
	// number of buckets in y axis (sizey)
	uint mxc_county;
	// total number of cells (size)
	uint mxc_count;

	// used when finding a matrix cell coordinates for point
	double limx_rcp; // 1/size of abscise of bucket //NOTE TODO REPLACE WITH *32768 (or <<15) and then >>15, use uint
	double limy_rcp; // 1/size of ordinate of bucket
} point_mx_type;


// initialises matrix
// defvx, defvy - default velocity of points
// life - maximum point lifetime in milliseconds
// size - maximum total number of points
// limx, limy - size of the playground
// mxc_countx, mxc_county - number of matrix cells in x, y axes
// size - max number of points in each matrix cell
void* pt_mx_init(double defvx, double defvy, uint life, uint size, uint limx, uint limy, uint mxc_countx, uint mxc_county, uint mxc_size);
// frees pt_mx and returns NULL
void* pt_mx_free(point_mx_type *pt_mx);

// PRIVATE calculates index of the matrix cell according to (x; y) position of point
// pt_mx - the matrix
// x, y - point coordinates
// xi, yi - matrix cell indexes/coordinates
// returns trough "xi, yi"
void pt_mx_cell_index(point_mx_type *pt_mx, double x, double y, uint *xi, uint *yi);
// recalculates positions of points within matrix, redistributes if necessary
// pt_mx - the matrix
void pt_mx_redistribute(point_mx_type *pt_mx);

// add a new point to the matrix
// pt_mx - the matrix
// x, y - location of the point
// vx, vy - velocity of the point
uint pt_mx_point(point_mx_type *pt_mx, double x, double y, double vx, double vy);
// delete "index" particle from the matrix
// pt_mx - the matrix
// index - used_stack index in pt_mx.h
void pt_mx_delete(point_mx_type *pt_mx, uint index);
void pt_mx_cell_delete(point_mx_type *pt_mx, point_mx_cell_type *pt_mxc);

// perform movement calculations with points in the matrix
// pt_mx - the matrix
void pt_mx_move(point_mx_type *pt_mx, double dt);
// performs collision calculations with points in the matrix
// only points in adjacent cells can collide
// pt_mx - the matrix
void pt_mx_collision(point_mx_type *pt_mx, double dt);
// performs boundary collision(deletion) calculations with points in the matrix
// only points in cells adjacent to boundaries can collide
// pt_mx - the matrix
// lim[] - boundary/limit coordinates, x0 y0 x1 y1
// klim - boundary/limit kill flag
void pt_mx_limit(point_mx_type *pt_mx, double lim[], double dt);

// iterate trough points and check if they should be deleted
// decrement timer of living points
// pt_mx - point matrix to check
void pt_mx_lifetime(point_mx_type *pt_mx);
// pt_mx_move + pt_mx_lifetime
// updates lifetime of pt_mx points
// deletes dead points
// moves alive points
// pt_mx - the matrix
// dt - delta time
void pt_mx_life_move(point_mx_type *pt_mx, double dt);

#endif
