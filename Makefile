#OBJS specifies which files to compile as part of the project
OBJS = point.c point_mx_cell.c point_mx.c airfoil_NACA4.c timer.c source.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -Wall output all warnings
# -w suppresses all warnings
# -O3 optimisation with standart adherence
# -Ofast optimisation, no standart adherence
# -Waggressive-loop-optimizations
# -Winline
COMPILER_FLAGS = -Wall -Ofast -ffast-math

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lm

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = program

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
