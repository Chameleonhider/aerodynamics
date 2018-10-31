/* © Algirdas Palubinskas */
/* Fair Use */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"
#include "point_mx_cell.h"

void pt_mxc_init(point_mx_cell_type *pt_mxc, uint size){
	pt_mxc->point_ind = calloc(size, sizeof(uint));
	pt_mxc->stack_count = 0;
	pt_mxc->size = size;
}
void* pt_mxc_free(point_mx_cell_type *pt_mxc){
	free(pt_mxc->point_ind);
	pt_mxc->point_ind = NULL;
	free(pt_mxc);
	pt_mxc = NULL;

	return pt_mxc;
}

uint pt_mxc_point(point_mx_cell_type *pt_mxc, uint point_ind){
	// // NOTE TODO REMOVE THIS CHECK (dangerous)
	// if (pt_mxc->stack_count >= pt_mxc->size){
	// 	printf("%s:%i: pt_mxc_point(): STACK SIZE NOT BIG ENOUGH\n", __FILE__, __LINE__);
	// 	return 0;
	// }

	uint index = pt_mxc->stack_count;
	pt_mxc->point_ind[index] = point_ind;
	++(pt_mxc->stack_count);
	return index;
}
void pt_mxc_delete(point_type *point, point_mx_cell_type *pt_mxc, uint index){
	--(pt_mxc->stack_count);
	pt_mxc->point_ind[index] = pt_mxc->point_ind[pt_mxc->stack_count];
	point[pt_mxc->point_ind[index]].mxc_ind = index;
}

void pt_mxc_collision(point_type *point, point_mx_cell_type *pt_mxc_a, point_mx_cell_type *pt_mxc_b, double dt){
	uint a;
	uint b;
	for (a=0; a<(pt_mxc_a->stack_count); ++a){
		for (b=0; b<(pt_mxc_b->stack_count); ++b){
			point_collision(&point[pt_mxc_a->point_ind[a]], &point[pt_mxc_b->point_ind[b]], dt);
		}
	}
}
void pt_mxc_collision_self(point_type *point, point_mx_cell_type *pt_mxc, double dt){
	uint a;
	uint b;
	for (a=0; a<(pt_mxc->stack_count); ++a){
		for (b=a+1; b<(pt_mxc->stack_count); ++b){
			point_collision(&point[pt_mxc->point_ind[a]], &point[pt_mxc->point_ind[b]], dt);
		}
	}
}
