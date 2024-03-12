all:
	g++ *.cpp -o artemide -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -I~/raylib/build/raylib/include/
	./artemide
