/* © Algirdas Palubinskas */
/* Fair Use */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #include <math.h> //gcc -lm
// #include <pthread.h> //gcc -pthread
// #include <unistd.h> //fork(), exec(), sleep()
// #include <sys/wait.h> //waitpid()
// #include <sched.h> //sheduler, sched_yield()
// #include <errno.h> //errno

//Using SDL, SDL_image
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "timer.h"
#include "point.h"
#include "point_mx_cell.h"
#include "point_mx.h"
#include "airfoil_NACA4.h"

const uint SCREEN_WIDTH = 300;
const uint SCREEN_HEIGHT = 150;
const uint SCREEN_ZOOM = 2;

void main_cycle();
void rotate_airfoil(point_mx_type *pt_mx, uint *point_airfoil, uint point_airfoil_count, double cx, double cy, double angle);
double gen_random(double low, double high);
int wrap_sdl_init();
void wrap_sdl_close();

SDL_Event e;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
const double hitsize = 10.0;

int main (int argc, char *argv[]) {
	printf("%s:%i\n", __FILE__, __LINE__);
	srand(time(NULL));

	if (!wrap_sdl_init()){
		printf("Failed to initialise SDL2\n");
		exit(EXIT_FAILURE);
	} else {
		main_cycle();
	}

	exit(EXIT_SUCCESS);
}

void main_cycle(){
	// cycle logic variables
	timer_type *timer = timer_init();
	uint it = 0;
	uint i;
	uint u;

	// point matrix variables
	uint size = 50000;
	uint life = 25000;
	uint mxc_size = 400; //max points per matrix cell
	double limits[4] = {hitsize*0.5, hitsize*0.1, SCREEN_WIDTH-hitsize*0.5, SCREEN_HEIGHT-hitsize*0.1};

	// physics variables
	const double spawnsize = 0.3;
	const double spawn_xdiff = 0.866025;
	const double defvx = -3.0; // default velocity
	const double defvy = 0.0;
	const double dt=1.0; // control simulation speed
	double dt_coll = 0.05*dt;
	double dt_lim = 0.2*dt;
	double dt_move = 0.01*dt;

	// point starting system
	char spawn_it = 0;
	double spawn_last_dist = 0.0;
	double spawn_0 = (limits[1] - spawnsize*hitsize);
	double spawn_1 = (limits[3] + spawnsize*hitsize);
	double spawn_dst = spawnsize*hitsize;
	double spawn_y;
	double spawn_x;

	uint col = 0x00000000;
	uint col0 = 0x00000000;
	uint col1 = 0x00C0C0C0;
	uint col_num = 0;
	uint col_it_switch = 5;
	uint col_it = 0;

	// point spawn mouse
	double mouse_x=0.0;
	double mouse_y=0.0;
	char mouse_left=0;
	char mouse_middle=0;
	char mouse_right=0;
	uint mouse_mxc_x;
	uint mouse_mxc_y;
	// other SDL inputs
	char skipAhead=0;
	double angle=0;
	double angle_d=0.01;

	// point matrix initialisation
	point_mx_type *pt_mx = pt_mx_init(defvx, defvy, life, size, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH/(hitsize*1.0), SCREEN_HEIGHT/(hitsize*1.0), mxc_size);

	// void* airfoil_naca4_gen(point_mx_type *pt_mx, uint point_count, double scale, double M, double P, double T);
	uint point_airfoil_count = 1000; // %2==0
	double airfoil_scale = 150.0;
	double airfoil_x = (SCREEN_WIDTH+airfoil_scale)*0.5;
	double airfoil_y = SCREEN_HEIGHT*0.5;
	// matrix, points in airfoil, start x, start y, camber, camber pos, thickness
	// uint *point_airfoil = airfoil_naca4_gen(pt_mx, point_airfoil_count, airfoil_scale, airfoil_x, airfoil_y, 0.03, 0.4, 0.25);
	uint *point_airfoil = airfoil_naca4_gen(pt_mx, point_airfoil_count, airfoil_scale, airfoil_x, airfoil_y, 0.1, 0.4, 0.1);
	double vx_sum;
	double vy_sum;

	while (1){
		timer_start(timer);
		//Handle events on queue
		while(SDL_PollEvent(&e) != 0){
			switch (e.type){
				default:
					break;
				case SDL_QUIT:{
					printf("Quitting\n");
					return;
					break;
				}
				case SDL_KEYDOWN:{
					//User requests quit
					switch (e.key.keysym.sym){
						case SDLK_q:{
							printf("Key pressed: Q\nQuitting\n");
							return;
							break;
						}
						case SDLK_SPACE:{
							skipAhead = 1;
							break;
						}
						case SDLK_UP:{
							angle = angle_d;
							break;
						}
						case SDLK_DOWN:{
							angle = angle_d*(-1.0);
							break;
						}
						default:
							break;
					}
					break;
				}
				case SDL_KEYUP:{
					switch (e.key.keysym.sym){
						case SDLK_SPACE:{
							// skip frames
							skipAhead = 0;
							break;
						}
						case SDLK_UP:{
							angle = 0.0;
							break;
						}
						case SDLK_DOWN:{
							angle = 0.0;
							break;
						}
						default:
							break;
					}
					break;
				}
				case SDL_MOUSEMOTION:{
					mouse_x = e.motion.x*1.0/SCREEN_ZOOM;
					mouse_y = e.motion.y*1.0/SCREEN_ZOOM;
					break;
				}
				case SDL_MOUSEBUTTONDOWN:{
					if (e.button.button == SDL_BUTTON_LEFT){
						mouse_left = 1;
					} else if (e.button.button == SDL_BUTTON_MIDDLE){
						mouse_middle = 1;
					} else if (e.button.button == SDL_BUTTON_RIGHT){
						mouse_right = 1;
					}
					break;
				}
				case SDL_MOUSEBUTTONUP:{
					if (e.button.button == SDL_BUTTON_LEFT){
						mouse_left = 0;
					} else if (e.button.button == SDL_BUTTON_MIDDLE){
						mouse_middle = 0;
					} else if (e.button.button == SDL_BUTTON_RIGHT){
						mouse_right = 0;
					}
					break;
				}
			}
		}

		if (angle != 0.0){
			// void rotate_airfoil(point_mx_type *pt_mx, uint *point_airfoil, uint point_airfoil_count, double cx, double cy, double angle);
			rotate_airfoil(pt_mx, point_airfoil, point_airfoil_count, airfoil_x, airfoil_y, angle);
			angle = 0.0;
		}

		if (mouse_left == 1 && it % 3 == 0){
			pt_mx_point_col(pt_mx, mouse_x, mouse_y+gen_random(-0.5, 0.5), defvx*2.0, gen_random(-0.2, 0.2), 0x0000FF00);
		} else if (mouse_middle) {
			pt_mx_cell_index(pt_mx, mouse_x, mouse_y, &mouse_mxc_x, &mouse_mxc_y);
			point_mx_cell_type *mxc = &pt_mx->cell_x[mouse_mxc_x][mouse_mxc_y];
			for (i=0; i<(mxc->stack_count); ++i){
				// pt_mx->point[mxc->point_ind[i]].vx += defvx*10.0*dt_move;
				// pt_mx->point[mxc->point_ind[i]].vy += defvy*10.0*dt_move;
				pt_mx->point[mxc->point_ind[i]].col = 0x00FF00FF;
			}
		} else if (mouse_right == 1){
			pt_mx_cell_index(pt_mx, mouse_x, mouse_y, &mouse_mxc_x, &mouse_mxc_y);
			pt_mx_cell_delete(pt_mx, &pt_mx->cell_x[mouse_mxc_x][mouse_mxc_y]);
			// printf("Mouse xy %i %i => %u %u\n", (int)mouse_x, (int)mouse_y, mouse_mxc_x, mouse_mxc_y);
		}

		// point spawning
		spawn_last_dist = spawn_last_dist - dt_move*defvx;
		if (spawn_last_dist > spawn_xdiff*spawn_dst){
			spawn_last_dist = spawn_last_dist - spawn_xdiff*spawn_dst;

			++col_it;
			if (col_it > col_it_switch + col_num*col_it_switch*2){
				col_it = 0;
				if (col_num == 0){
					col_num = 1;
					col = col1;
				} else {
					col_num = 0;
					col = col0;
				}
			}

			// iterations are spawn_xdiff*spawnsize*hitsize apart on X axis
			spawn_x = limits[2]+spawn_dst-spawn_last_dist;
			spawn_x = limits[2]+hitsize;
			if (spawn_it == 0){
				spawn_it = 1;
				for (spawn_y=spawn_0; spawn_y<spawn_1; spawn_y=spawn_y+spawn_dst){
					pt_mx_point_col(pt_mx, spawn_x, spawn_y, defvx, defvy, col);
				}
			} else {
				spawn_it = 0;
				for (spawn_y=spawn_1; spawn_y>spawn_0; spawn_y=spawn_y-spawn_dst){
					pt_mx_point_col(pt_mx, spawn_x, spawn_y, defvx, defvy, col);
				}
			}
		}

		pt_mx_collision(pt_mx, dt_coll);
		pt_mx_limit(pt_mx, limits, dt_lim);
		// TODO make a new function "pt_mx_exclude" to zero the velocity of points
		vx_sum=0.0;
		vy_sum=0.0;
		for (i=0; i<point_airfoil_count; ++i){
			vx_sum = vx_sum + pt_mx->point[point_airfoil[i]].vx;
			vy_sum = vy_sum + pt_mx->point[point_airfoil[i]].vy;
			pt_mx->point[point_airfoil[i]].vx = 0.0;
			pt_mx->point[point_airfoil[i]].vy = 0.0;
			pt_mx->point[point_airfoil[i]].time = 99;
		}
		pt_mx_life_move(pt_mx, dt_move);
		pt_mx_redistribute(pt_mx);

		if (skipAhead == 0){

			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF );
			uint col;
			for (i=0; i<(pt_mx->stack_count); ++i){
				u = pt_mx->used_stack[i];
				col = pt_mx->point[u].col;
				SDL_SetRenderDrawColor(renderer, (col >> 16)&0xFF, (col >> 8)&0xFF, (col)&0xFF, 0xFF);
				SDL_RenderDrawPoint(renderer, pt_mx->point[u].x*SCREEN_ZOOM, pt_mx->point[u].y*SCREEN_ZOOM);
				SDL_RenderDrawPoint(renderer, pt_mx->point[u].x*SCREEN_ZOOM+1, pt_mx->point[u].y*SCREEN_ZOOM+1);
				SDL_RenderDrawPoint(renderer, pt_mx->point[u].x*SCREEN_ZOOM, pt_mx->point[u].y*SCREEN_ZOOM+1);
				SDL_RenderDrawPoint(renderer, pt_mx->point[u].x*SCREEN_ZOOM+1, pt_mx->point[u].y*SCREEN_ZOOM);
			}
			// SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
			// SDL_RenderDrawPoint(renderer, mouse_x*SCREEN_ZOOM, mouse_y*SCREEN_ZOOM);

			SDL_RenderPresent(renderer);
		}

		if (it % 5 == 0){
			printf("Time %u, points %i, vel %g %g\n", timer_ticks_running(timer), pt_mx->stack_count, vx_sum, vy_sum);
		}

		++it;
	}
}

void rotate_airfoil(point_mx_type *pt_mx, uint *point_airfoil, uint point_airfoil_count, double cx, double cy, double angle){
	double sin_a = sin(angle);
	double cos_a = cos(angle);

	double x;
	double y;

	uint i;
	point_type *pt;
	for (i=0; i<point_airfoil_count; ++i){
		pt = &pt_mx->point[point_airfoil[i]];

		// translate point back to origin:
		pt->x = pt->x - cx;
		pt->y = pt->y - cy;

		// rotate point
		x = pt->x*cos_a - pt->y*sin_a;
		y = pt->x*sin_a + pt->y*cos_a;

		// translate point back:
		pt->x = x + cx;
		pt->y = y + cy;
	}
	pt_mx_redistribute(pt_mx);
}
double gen_random(double low, double high){
	double random = rand()*1.0/RAND_MAX;
	random = random*(high-low);
	random = random+low;
	return random;
}

int wrap_sdl_init(){
	//Initialisation flag
	int success = 1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialise! SDL Error: %s\n", SDL_GetError());
		success = 0;
	} else {
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCREEN_ZOOM, SCREEN_HEIGHT*SCREEN_ZOOM, SDL_WINDOW_SHOWN);
		if (window == NULL){
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = 0;
		} else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL){
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = 0;
			} else {
				//Initialise renderer color
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	printf("%s:%i\n", __FILE__, __LINE__);
	return success;
}
void wrap_sdl_close(){
	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	// IMG_Quit();
	SDL_Quit();
}
