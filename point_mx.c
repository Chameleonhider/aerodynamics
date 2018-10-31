/* © Algirdas Palubinskas */
/* Fair Use */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"
#include "point_mx_cell.h"
#include "point_mx.h"

void* pt_mx_init(double defvx, double defvy, uint life, uint size, uint limx, uint limy, uint mxc_countx, uint mxc_county, uint mxc_size){
	point_mx_type *pt_mx = calloc(1, sizeof(point_mx_type));

	pt_mx->point = calloc(size, sizeof(point_type));
	pt_mx->used_stack = calloc(size, sizeof(uint));
	pt_mx->free_stack = calloc(size, sizeof(uint));
	pt_mx->stack_count = 0;
	pt_mx->size = size;

	pt_mx->defvx = defvx;
	pt_mx->defvy = defvy;
	pt_mx->life = life;

	for (uint i=0; i<size; ++i){
		pt_mx->free_stack[i] = i;
	}

	// lim/size == size of bucket in coordinates
	pt_mx->limx_rcp = (mxc_countx*1.0+0.1)/limx;
	pt_mx->limy_rcp = (mxc_county*1.0+0.1)/limy;

	pt_mx->mxc_countx = mxc_countx;
	pt_mx->mxc_county = mxc_county;
	pt_mx->mxc_count = mxc_countx*mxc_county;

	pt_mx->cell_main = calloc(pt_mx->mxc_count, sizeof(point_mx_cell_type));
	pt_mx->cell_x = calloc(mxc_countx, sizeof(point_mx_cell_type*));

	for (uint i=0; i<(pt_mx->mxc_count); ++i){
		pt_mxc_init(&pt_mx->cell_main[i], mxc_size);
	}
	for (uint x=0; x<mxc_countx; ++x){
		pt_mx->cell_x[x] = &pt_mx->cell_main[x*mxc_county];
	}

	printf("%s:%i: pt_mx_init: Initialised matrix. Count %u x %u y %u\n", __FILE__, __LINE__, pt_mx->mxc_count, pt_mx->mxc_countx, pt_mx->mxc_county);
	return pt_mx;
}
void* pt_mx_free(point_mx_type *pt_mx){
	free(pt_mx->point);
	pt_mx->point = NULL;
	free(pt_mx->used_stack);
	pt_mx->used_stack = NULL;
	free(pt_mx->free_stack);
	pt_mx->free_stack = NULL;
	free(pt_mx->cell_main);
	pt_mx->cell_main = NULL;
	free(pt_mx->cell_x);
	pt_mx->cell_x = NULL;

	free(pt_mx);
	pt_mx = NULL;

	return pt_mx;
}

void pt_mx_cell_index(point_mx_type *pt_mx, double x, double y, uint *xi, uint *yi){
	x = fmax(x, 0.0);
	y = fmax(y, 0.0);
	*xi = (uint)(x*(pt_mx->limx_rcp));
	*yi = (uint)(y*(pt_mx->limy_rcp));

	if (*xi >= (pt_mx->mxc_countx)){
		*xi = (pt_mx->mxc_countx)-1;
	}
	if (*yi >= (pt_mx->mxc_county)){
		*yi = (pt_mx->mxc_county)-1;
	}
}
// NOTE TODO testing
void pt_mx_redistribute(point_mx_type *pt_mx){
	uint xi;
	uint yi;

	uint i;
	uint u;
	for (i=0; i<(pt_mx->stack_count); ++i){
		u = pt_mx->used_stack[i];
		pt_mx_cell_index(pt_mx, pt_mx->point[u].x, pt_mx->point[u].y, &xi, &yi);

		if (xi != pt_mx->point[u].mxc_xind || yi != pt_mx->point[u].mxc_yind){
			pt_mxc_delete(pt_mx->point, &pt_mx->cell_x[pt_mx->point[u].mxc_xind][pt_mx->point[u].mxc_yind], pt_mx->point[u].mxc_ind);
			pt_mx->point[u].mxc_ind = pt_mxc_point(&pt_mx->cell_x[xi][yi], u);
			pt_mx->point[u].mxc_xind = xi;
			pt_mx->point[u].mxc_yind = yi;
		}

	}
}

uint pt_mx_point(point_mx_type *pt_mx, double x, double y, double vx, double vy){
	if (pt_mx->stack_count == pt_mx->size){
		printf("%s:%i: pt_mx_point(): Matrix ran out of points.\n", __FILE__, __LINE__);
		return;
	}
	uint index = pt_mx->free_stack[pt_mx->stack_count];
	pt_mx->used_stack[pt_mx->stack_count] = index;
	++(pt_mx->stack_count);

	pt_mx->point[index].x = x;
	pt_mx->point[index].y = y;
	pt_mx->point[index].vx = vx;
	pt_mx->point[index].vy = vy;
	pt_mx->point[index].time = pt_mx->life;
	pt_mx_cell_index(pt_mx, x, y, &pt_mx->point[index].mxc_xind, &pt_mx->point[index].mxc_yind);
	pt_mx->point[index].mxc_ind = pt_mxc_point(&pt_mx->cell_x[pt_mx->point[index].mxc_xind][pt_mx->point[index].mxc_yind], index);
	return index;
}
void pt_mx_delete(point_mx_type *pt_mx, uint index){
	--(pt_mx->stack_count);

	// if the main array index is not in it's place
	if (pt_mx->used_stack[index] > pt_mx->stack_count){
		pt_mx->free_stack[pt_mx->stack_count] = pt_mx->free_stack[pt_mx->used_stack[index]];
		pt_mx->free_stack[pt_mx->used_stack[index]] = pt_mx->used_stack[index];
	} else {
		pt_mx->free_stack[pt_mx->stack_count] = pt_mx->used_stack[index];
	}

	// pt_mx->free_stack[pt_mx->stack_count] = pt_mx->used_stack[index];
	pt_mx->used_stack[index] = pt_mx->used_stack[pt_mx->stack_count];
}
void pt_mx_cell_delete(point_mx_type *pt_mx, point_mx_cell_type *pt_mxc){
	uint p;
	for (p=0; p<(pt_mxc->stack_count); ++p){
		pt_mx->point[pt_mxc->point_ind[p]].time = 0;
	}
}

// NOTE TODO -- test if sequential access of the main array is faster
void pt_mx_move(point_mx_type *pt_mx, double dt){
	uint i;
	uint u;
	for (i=0; i<(pt_mx->stack_count); ++i){
		u = pt_mx->used_stack[i];

		// point_move(&pt_mx->point[u], dt);
		pt_mx->point[u].x = pt_mx->point[u].x + pt_mx->point[u].vx*dt;
		pt_mx->point[u].y = pt_mx->point[u].y + pt_mx->point[u].vy*dt;
	}
}
// NOTE TODO -- try colliding "observer" points with sums of mx cells
void pt_mx_collision(point_mx_type *pt_mx, double dt){
	uint mxc_countx = pt_mx->mxc_countx;
	uint mxc_county = pt_mx->mxc_county;
	uint xi;
	uint yi;

	// bottom left
	for (xi=1; xi<(mxc_countx); ++xi){
		for (yi=0; yi<(mxc_county-1); ++yi){
			pt_mxc_collision(pt_mx->point, &pt_mx->cell_x[xi][yi], &pt_mx->cell_x[xi-1][yi+1], dt);
		}
	}
	// bottom
	for (xi=0; xi<(mxc_countx); ++xi){
		for (yi=0; yi<(mxc_county-1); ++yi){
			pt_mxc_collision(pt_mx->point, &pt_mx->cell_x[xi][yi], &pt_mx->cell_x[xi][yi+1], dt);
		}
	}
	// bottom right
	for (xi=0; xi<(mxc_countx-1); ++xi){
		for (yi=0; yi<(mxc_county-1); ++yi){
			pt_mxc_collision(pt_mx->point, &pt_mx->cell_x[xi][yi], &pt_mx->cell_x[xi+1][yi+1], dt);
		}
	}
	// right
	for (xi=0; xi<(mxc_countx-1); ++xi){
		for (yi=0; yi<(mxc_county); ++yi){
			pt_mxc_collision(pt_mx->point, &pt_mx->cell_x[xi][yi], &pt_mx->cell_x[xi+1][yi], dt);
		}
	}
	// self-collision
	for (xi=0; xi<(pt_mx->mxc_count); ++xi){
		pt_mxc_collision_self(pt_mx->point, &pt_mx->cell_main[xi], dt);
	}
}
void pt_mx_limit(point_mx_type *pt_mx, double lim[], double dt){
	uint countx_m1 = (pt_mx->mxc_countx)-1;
	uint county_m1 = (pt_mx->mxc_county)-1;

	uint i;
	uint p;
	point_type *pt = pt_mx->point;

	// boundary columns
	for (i=0; i<(pt_mx->mxc_county); ++i){
		// left boundary column
		for (p=0; p<(pt_mx->cell_x[0][i].stack_count); ++p){
			point_limit_xlower(&pt[pt_mx->cell_x[0][i].point_ind[p]], lim[0], pt_mx->defvx, pt_mx->defvy, dt);
			// point_limit(&pt[pt_mx->cell_x[0][i].point_ind[p]], lim, klim, dt);
		}
		// right boundary column
		for (p=0; p<(pt_mx->cell_x[countx_m1][i].stack_count); ++p){
			point_limit_xupper(&pt[pt_mx->cell_x[countx_m1][i].point_ind[p]], lim[2], pt_mx->defvx, pt_mx->defvy, dt);
			// point_limit(&pt[pt_mx->cell_x[countx_m1][i].point_ind[p]], lim, klim, dt);
		}
	}

	// boundary rows
	for (i=0; i<(pt_mx->mxc_countx); ++i){
		// upper boundary row
		for (p=0; p<(pt_mx->cell_x[i][0].stack_count); ++p){
			point_limit_ylower(&pt[pt_mx->cell_x[i][0].point_ind[p]], lim[1], pt_mx->defvx, pt_mx->defvy, dt);
			// point_limit(&pt[pt_mx->cell_x[i][0].point_ind[p]], lim, klim, dt);
		}
		// lower boundary row
		for (p=0; p<(pt_mx->cell_x[i][county_m1].stack_count); ++p){
			point_limit_yupper(&pt[pt_mx->cell_x[i][county_m1].point_ind[p]], lim[3], pt_mx->defvx, pt_mx->defvy, dt);
			// point_limit(&pt[pt_mx->cell_x[i][county_m1].point_ind[p]], lim, klim, dt);
		}
	}
}

void pt_mx_lifetime(point_mx_type *pt_mx){
	uint u;
	for (uint i=0; i<(pt_mx->stack_count); ++i){
		while (i < (pt_mx->stack_count)){
			u = pt_mx->used_stack[i];
			if (pt_mx->point[u].time == 0){
				pt_mxc_delete(pt_mx->point, &pt_mx->cell_x[pt_mx->point[u].mxc_xind][pt_mx->point[u].mxc_yind], pt_mx->point[u].mxc_ind);
				pt_mx_delete(pt_mx, i);
			} else {
				--(pt_mx->point[u].time);
				break;
			}
		}
	}
}
void pt_mx_life_move(point_mx_type *pt_mx, double dt){
	uint u;
	for (uint i=0; i<(pt_mx->stack_count); ++i){
		while (i < (pt_mx->stack_count)){
			u = pt_mx->used_stack[i];
			if (pt_mx->point[u].time == 0){
				pt_mxc_delete(pt_mx->point, &pt_mx->cell_x[pt_mx->point[u].mxc_xind][pt_mx->point[u].mxc_yind], pt_mx->point[u].mxc_ind);
				pt_mx_delete(pt_mx, i);
			} else {
				pt_mx->point[u].x = pt_mx->point[u].x + pt_mx->point[u].vx*dt;
				pt_mx->point[u].y = pt_mx->point[u].y + pt_mx->point[u].vy*dt;
				--(pt_mx->point[u].time);
				break;
			}
		}
	}
}
