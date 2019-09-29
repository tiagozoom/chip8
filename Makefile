main: main.o display_controller.o CPU.o
	g++ -o main main.cpp display_controller.cpp opcode.cpp CPU.cpp `sdl2-config --cflags --libs`
