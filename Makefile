#compilation flags
SDL_inc = -lSDL2 -lSDL2_image -lSDL2_ttf
c_flg = -std=c++11 -Wall

#valgrind flags
vtool = --tool=memcheck
vopt = --log-file="memory_check.txt" --leak-check=full

#object files
objects = main.o handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o derived_buttons.o

#executable name
name = andiamo

#controls which browser the makefile opens to view doxygen html manual
browser = firefox

andiamo: $(objects)
	g++  -o $(name) $(objects) $(SDL_inc)

main.o: main.cc sdl_help.h handlers.h input_maker.h button_manager.h
	g++ $(c_flg) -c main.cc

handlers.o: handlers.cc handlers.h sdl_help.h
	g++ $(c_flg) -c handlers.cc

sdl_help.o: sdl_help.cc sdl_help.h manager.h scroll_bar.h input_maker.h
	g++ $(c_flg) -c sdl_help.cc

scroll_bar.o: scroll_bar.cc scroll_bar.h
	g++ $(c_flg) -c scroll_bar.cc

field.o: field.cc field.h
	g++ $(c_flg) -c field.cc

button_manager.o: button_manager.cc button_manager.h button.h derived_buttons.h
	g++ $(c_flg) -c button_manager.cc

button.o: button.cc button.h derived_buttons.h
	g++ $(c_flg) -c button.cc
derived_buttons.o: derived_buttons.cc derived_buttons.h button.h
	g++ $(c_flg) -c derived_buttons.cc
manager.o: manager.cc manager.h field.h
	g++ $(c_flg) -c manager.cc

ftran_structs.o: ftran_structs.cc ftran_structs.h
	g++ $(c_flg) -c ftran_structs.cc

input_maker.o: input_maker.cc input_maker.h string+.h
	g++ $(c_flg) -c input_maker.cc

string.o: string+.cc string+.h
	g++ $(c_flg) -c string+.cc
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





