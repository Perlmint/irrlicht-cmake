CPP = g++
OPTS =  -I"../../include" -I"/usr/X11R6/include" -L"/usr/X11R6/lib" -L"../../lib/Linux" -lIrrlicht -lGL -lGLU -lXxf86vm -lXext -lX11

all:
	$(CPP) main.cpp -o example $(OPTS)

clean:
	$(RM) example
