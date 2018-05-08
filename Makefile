CPPFLAGS = -W -lGL -lGLU -lglut

all: main.cpp
	g++ $(CPPFLAGS) main.cpp -o main
