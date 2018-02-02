#compilation flags
SDL_INC = -lSDL2 -lSDL2_image -lSDL2_ttf
C_FLG = -std=c++11 -Wall

#valgrind flags
vtool = --tool=memcheck
vopt = --log-file="memory_check.txt" --leak-check=full

#object files
#objects = main.o handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o dqv.o deck.o text_box.o cursor.o logger.o asset_manager.o
#main_objects = handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o dqv.o deck.o text_box.o cursor.o logger.o asset_manager.o
objects = main.o handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o deck.o text_box.o cursor.o logger.o asset_manager.o
main_objects = handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o deck.o text_box.o cursor.o logger.o asset_manager.o



#header files
headers = button.h field.h ftran_structs.h logger.h sdl_help.h button_manager.h form_buttons.h handlers.h manager.h string+.h derived_buttons.h form.h input_maker.h scroll_bar.h text_box.h

#executable name
name = andiamo

#controls which browser the makefile opens to view doxygen html manual
browser = firefox

andiamo: $(objects)
	g++  -o $(name) $(objects) $(SDL_INC)

main.o: main.cc $(main_objects)
	g++ $(C_FLG) -c main.cc

handlers.o: handlers.cc handlers.h sdl_help.h button_manager.h
	g++ $(C_FLG) -c handlers.cc

sdl_help.o: sdl_help.cc sdl_help.h scroll_bar.o manager.o input_maker.o button_manager.o logger.o
	g++ $(C_FLG) -c sdl_help.cc

asset_manager.o: asset_manager.h asset_manager.cc
	g++ $(C_FLG) -c asset_manager.cc

scroll_bar.o: scroll_bar.cc scroll_bar.h logger.o
	g++ $(C_FLG) -c scroll_bar.cc

field.o: field.cc field.h ftran_structs.o string+.o logger.o
	g++ $(C_FLG) -c field.cc

button_manager.o: button_manager.cc button_manager.h button.o derived_buttons.o form_buttons.o logger.o
	g++ $(C_FLG) -c button_manager.cc

button.o: button.cc button.h derived_buttons.o
	g++ $(C_FLG) -c button.cc

derived_buttons.o: derived_buttons.cc derived_buttons.h logger.o
	g++ $(C_FLG) -c derived_buttons.cc

form_buttons.o: form_buttons.cc form_buttons.h button.o form.o logger.o c_tuples.h
	g++ $(C_FLG) -c form_buttons.cc

form.o: form.cc form.h button.o text_box.o c_tuples.h
	g++ $(C_FLG) -c form.cc

manager.o: manager.cc manager.h field.o input_maker.o logger.o
	g++ $(C_FLG) -c manager.cc

ftran_structs.o: ftran_structs.cc ftran_structs.h
	g++ $(C_FLG) -c ftran_structs.cc

input_maker.o: input_maker.cc input_maker.h string+.o ftran_structs.o string+.o button_manager.o
	g++ $(C_FLG) -c input_maker.cc

fop_handler.o: fop_handler.cc fop_handler.h deck.o
	g++ $(C_FLG) -c fop_handler.cc

#dqv.o: dqv.f
#	gfortran -c -o dqv.o -ffree-form dqv.f

deck.o: deck.h deck.cc
	g++ $(C_FLG) -c deck.cc

text_box.o: text_box.cc text_box.h sdl_help.h
	g++ $(C_FLG) -c text_box.cc

cursor.o: cursor.cc cursor.h
	g++ $(C_FLG) -c cursor.cc

logger.o: logger.cc logger.h
	g++ $(C_FLG) -c logger.cc

string.o: string+.cc string+.h
	g++ $(C_FLG) -c string+.cc
#runs valgrind on the debug executable created by make gdb
valgrind:
	valgrind $(vtool) $(vopt) ./debug
#compile for debugging, or when includes have gotten messed up
gdb:
	g++ -g -o debug $(C_FLG) *.cc $(SDL_INC)
#pack useful files up for an email or storage
tar:
	tar -czvf andiamo.tar.gz *.cc *.h Makefile doxyfile readme.md tile_Input Assets sandbox config sandbox output
#have doxygen run and create a manual from source comments and the configuration settings in doxyfile
doxy: doxyfile
	doxygen doxyfile
	$(browser) ./doxyout/html/index.html
#remove compiled things, text editor saves, memory check output
clean:
	rm *~ *.o andiamo debug memory_check.txt





