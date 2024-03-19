all:
	g++ *.cpp -o artemide -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -I~/raylib/build/raylib/include/
	./artemide ./circuits/parallel.circ

valgrind:
	g++ -g -Og *.cpp -o artemide -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -I~/raylib/build/raylib/include/
	valgrind ./artemide 
	# valgrind --leak-check=full ./artemide 
