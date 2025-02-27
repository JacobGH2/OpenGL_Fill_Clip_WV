all:
	g++ --std=c++11 main.cpp -o a.out -framework OpenGL -framework GLUT -Wno-deprecated
	./a.out

clean:
	rm -f a.out