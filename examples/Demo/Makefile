# Irrlicht Engine Demo Makefile
# Handwritten by me, made with Kate. So it is far from being perfect. :)

CPP = g++
OPTS =  -I"../../include" -I"/usr/X11R6/include" -L"/usr/X11R6/lib" -L"../../lib/Linux" -lIrrlicht -lGL -lGLU -lXxf86vm -lXext -lX11 -lz -ljpeg

LINKOBJ  =  CDemo.o CMainMenu.o main.o $(RES)

all:
	$(CPP) main.cpp CDemo.cpp CMainMenu.cpp -o demo $(OPTS)

clean:
	rm demo
