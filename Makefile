main: main.o display_controller.o cpu.o
	g++ -o main main.cpp display_controller.cpp opcode.cpp cpu.cpp `sdl2-config --cflags --libs`
