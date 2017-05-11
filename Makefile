SDL_inc = -lSDL2 -lSDL2_image
c_flg = -std=c++11 -Wall
vtool = --tool=memcheck
vopt = --log-file="memory_check.txt" --leak-check=full
browser = firefox

andiamo: main.o handlers.o sdl_help.o manager.o field.o 
	g++  -o andiamo main.o handlers.o manager.o field.o sdl_help.o $(SDL_inc)

main.o: main.cc sdl_help.h handlers.h
	g++ $(c_flg) -c main.cc

handlers.o: handlers.cc handlers.h sdl_help.h
	g++ $(c_flg) -c handlers.cc

sdl_help.o: sdl_help.cc sdl_help.h manager.h
	g++ $(c_flg) -c sdl_help.cc

field.o: field.cc field.h
	g++ $(c_flg) -c field.cc

manager.o: manager.cc manager.h
	g++ $(c_flg) -c manager.cc

valgrind:
	valgrind $(vtool) $(vopt) ./andiamo

gdb:
	g++ -g -o debug $(c_flg) *.cc $(SDL_inc)

tar:
	tar -czvf andiamo.tar.gz *.cc *.h Makefile doxyfile readme.md tile_Input HF_Input Assets sandbox

doxy: doxyfile
	doxygen doxyfile
	$(browser) ./doxyout/html/index.html

clean:
	rm *~ *.o andiamo debug memory_check.txt
