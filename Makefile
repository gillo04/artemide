all:
	gcc main.c -o helloworld -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -I~/raylib/build/raylib/include/
	./helloworld
