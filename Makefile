#compilation flags
SDL_inc = -lSDL2 -lSDL2_image
c_flg = -std=c++11 -Wall

#valgrind flags
vtool = --tool=memcheck
vopt = --log-file="memory_check.txt" --leak-check=full

#controls which browser the makefile opens to view doxygen html manual
browser = firefox

andiamo: main.o handlers.o sdl_help.o scroll_bar.o manager.o field.o 
	g++  -o andiamo main.o handlers.o manager.o field.o sdl_help.o scroll_bar.o $(SDL_inc)

main.o: main.cc sdl_help.h handlers.h
	g++ $(c_flg) -c main.cc

handlers.o: handlers.cc handlers.h sdl_help.h
	g++ $(c_flg) -c handlers.cc

sdl_help.o: sdl_help.cc sdl_help.h manager.h scroll_bar.h
	g++ $(c_flg) -c sdl_help.cc

scroll_bar.o: scroll_bar.cc scroll_bar.h
	g++ $(c_flg) -c scroll_bar.cc

field.o: field.cc field.h
	g++ $(c_flg) -c field.cc

manager.o: manager.cc manager.h
	g++ $(c_flg) -c manager.cc
#runs valgrind on the debug executable created by make gdb
valgrind:
	valgrind $(vtool) $(vopt) ./debug
#compile for debugging, or when includes have gotten messed up
gdb:
	g++ -g -o debug $(c_flg) *.cc $(SDL_inc)
#pack useful files up for an email or storage
tar:
	tar -czvf andiamo.tar.gz *.cc *.h Makefile doxyfile readme.md tile_Input Assets sandbox
#have doxygen run and create a manual from source comments and the configuration settings in doxyfile
doxy: doxyfile
	doxygen doxyfile
	$(browser) ./doxyout/html/index.html
#remove compiled things, text editor saves, memory check output
clean:
	rm *~ *.o andiamo debug memory_check.txt
