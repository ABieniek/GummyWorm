#OBJS specifies which files to compile as part of the project
OBJS = desktopController.c

# stuff from https://www.libsdl.org/projects/SDL_image/docs/demos/Makefile
COMPILER = gcc
CFLAGS= -O2 -Wall $(shell sdl2-config --cflags)
LDFLAGS= -O2 -Wall $(shell sdl2-config --libs) -lSDL2_image
PROGRAMS= desktopController

#This is the target that compiles our executable
all : $(PROGRAMS)

$(PROGRAMS): $(OBJS)
	$(COMPILER) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@
