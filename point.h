/* © Algirdas Palubinskas */
/* Fair Use */

#ifndef POINT_TYPE_HEADER_
#define POINT_TYPE_HEADER_

typedef struct point_type {
	// physics
	double x;
	double y;
	double vx;
	double vy;
	uint time; // time in milliseconds lived

	// information
	uint mxc_xind; // x index of point cointainer
	uint mxc_yind; // y index of point cointainer
	uint mxc_ind;
} point_type;

// move the point (manually inline to make it go faster)
// pt - the point
void point_move(point_type *pt, double dt);
// collide two points
// *pt0 - point_type number 0
// *pt1 - point_type number 1
void point_collision(point_type *pt0, point_type *pt1, double dt);

void point_limit_xlower(point_type *point, double lim, double defvx, double defvy, double dt);
void point_limit_xupper(point_type *point, double lim, double defvx, double defvy, double dt);
void point_limit_ylower(point_type *point, double lim, double defvx, double defvy, double dt);
void point_limit_yupper(point_type *point, double lim, double defvx, double defvy, double dt);

// // limit point to lim[] area
// // *pts - point_array
// // i - point_type index within the *pts TODO - replace with pointer to point_type struct
// // lim[] - single-dimension, length 4 array with limits [x0, y0, x1, y1]. upleft is (0; 0)
// // klim - boundary kill flag (set time to zero)
// void point_limit(point_type *pt, double lim[], int klim, double dt);

#endif
