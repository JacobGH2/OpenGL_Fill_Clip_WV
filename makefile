CFLAGS = --std=c++11

# EXECUTABLE

all: main.o clip.o fill.o
	g++ --std=c++11 main.o clip.o fill.o -o a.out -framework OpenGL -framework GLUT -Wno-deprecated
	./a.out

# OBJECT

main.o: main.cpp proj.h
	g++ ${CFLAGS} main.cpp -c -Wno-deprecated

clip.o: clip.cpp proj.h
	g++ ${CFLAGS} clip.cpp -c -Wno-deprecated

fill.o: fill.cpp proj.h
	g++ ${CFLAGS} fill.cpp -c -Wno-deprecated

# UTILITY

clean:
	rm -f a.out *.o