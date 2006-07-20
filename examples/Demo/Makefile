# Irrlicht Engine Demo Makefile
# Handwritten by me, made with Kate. So it is far from being perfect. :)

CXX = g++
CPPFLAGS = -I"../../include" -I"/usr/X11R6/include"
CXXFLAGS = -O3 -ffast-math
LDFLAGS = -L"/usr/X11R6/lib" -L"../../lib/Linux" -lIrrlicht -lGL -lGLU -lXxf86vm -lXext -lX11
# if you enable sound add the proper library for linking
# -laudiere
# -lSDL_mixer -lSDL

LINKOBJ = CDemo.o CMainMenu.o main.o $(RES)

all: demo

demo: $(LINKOBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o demo $(LDFLAGS)

clean:
	$(RM) $(LINKOBJ) demo
