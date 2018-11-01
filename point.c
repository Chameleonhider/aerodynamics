/* © Algirdas Palubinskas */
/* Fair Use */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"

const double hitsize;

void point_move(point_type *pt, double dt){
	pt->x = pt->x + pt->vx*dt;
	pt->y = pt->y + pt->vy*dt;
}
void point_collision(point_type *pt0, point_type *pt1, double dt){
	if (pt0 == pt1){
		return;
	}

	// make a vector from 0 to 1
	double dx = pt1->x - pt0->x;
	double dy = pt1->y - pt0->y;

	double dist = dx*dx + dy*dy;
	// double dist = fabs(dx) + fabs(dy);
	dist = sqrt(dist);
	if (dist > hitsize){
		return;
	}

	double force;
	force = (hitsize-dist)*dt/hitsize;

	if (dx < 0){
		pt0->vx += force;
		pt1->vx -= force;
	} else {
		pt0->vx -= force;
		pt1->vx += force;
	}
	if (dy < 0){
		pt0->vy += force;
		pt1->vy -= force;
	} else {
		pt0->vy -= force;
		pt1->vy += force;
	}

	// this costs 100points/ms
	force = force*0.05;
	dist = (1.0 - force);
	dx = pt0->vx*dist + force*pt1->vx;
	dy = pt1->vx*dist + force*pt0->vx;
	pt0->vx = dx;
	pt1->vx = dy;
	dx = pt0->vy*dist + force*pt1->vy;
	dy = pt1->vy*dist + force*pt0->vy;
	pt0->vy = dx;
	pt1->vy = dy;
}

const double kill_lower = 1.0;
const double kill_upper = -1.0;
void point_limit_xlower(point_type *point, double lim, double defvx, double defvy, double dt){
	double delta;

	delta = lim - point->x;
	if (delta < 0.0){
		return;
	}
	if (delta > hitsize*kill_lower){
		point->time = 0;
		return;
	}

	double defv_part = delta*dt*(1.0)/hitsize;
	point->vx = point->vx*(1.0 - defv_part) + defvx*defv_part;
	defv_part = defv_part*(0.5);
	point->vy = point->vy*(1.0 - defv_part) + defvy*defv_part;
}
void point_limit_xupper(point_type *point, double lim, double defvx, double defvy, double dt){
	double delta;

	delta = lim - point->x;
	if (delta > 0.0){
		return;
	}
	if (delta < hitsize*kill_upper){
		point->time = 0;
		return;
	}

	double defv_part = delta*dt*(-1.0)/hitsize;
	point->vx = point->vx*(1.0 - defv_part) + defvx*defv_part;
	defv_part = defv_part*(0.5);
	point->vy = point->vy*(1.0 - defv_part) + defvy*defv_part;
}
void point_limit_ylower(point_type *point, double lim, double defvx, double defvy, double dt){
	double delta;

	delta = lim - point->y;
	if (delta < 0.0){
		return;
	}
	if (delta > hitsize*kill_lower){
		point->time = 0;
		return;
	}

	double defv_part = delta*dt*(1.0)/hitsize;
	point->vy = point->vy*(1.0 - defv_part) + defvy*defv_part;
	defv_part = defv_part*(0.5);
	point->vx = point->vx*(1.0 - defv_part) + defvx*defv_part;
}
void point_limit_yupper(point_type *point, double lim, double defvx, double defvy, double dt){
	double delta;

	delta = lim - point->y;
	if (delta > 0.0){
		return;
	}
	if (delta < hitsize*kill_upper){
		point->time = 0;
		return;
	}

	double defv_part = delta*dt*(-1.0)/hitsize;
	point->vy = point->vy*(1.0 - defv_part) + defvy*defv_part;
	defv_part = defv_part*(0.5);
	point->vx = point->vx*(1.0 - defv_part) + defvx*defv_part;
}
