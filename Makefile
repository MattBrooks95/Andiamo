#compilation flags
SDL_INC = -lSDL2 -lSDL2_image -lSDL2_ttf
C_FLG   = -std=c++11 -Wall

#valgrind flags
vtool = --tool=memcheck
vopt  = --log-file="memory_check.txt" --leak-check=full

#object files

F_OBJS       = ./q_val/cqvalue.o ./q_val/dqvalu.o ./q_val/dmass.o ./q_val/bamt16.o
OBJECTS      = main.o handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o deck.o text_box.o cursor.o logger.o asset_manager.o helpers.o
MAIN_OBJECTS = handlers.o manager.o ftran_structs.o field.o sdl_help.o scroll_bar.o input_maker.o string+.o button_manager.o button.o form_buttons.o form.o derived_buttons.o fop_handler.o deck.o text_box.o cursor.o logger.o asset_manager.o helpers.o

#executable name
name = andiamo

#controls which browser the makefile opens to view doxygen html manual
browser = firefox

andiamo: $(OBJECTS)
	g++  -o $(name) $(OBJECTS) $(F_OBJS) $(SDL_INC)

main.o: main.cc $(MAIN_OBJECTS)
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

input_maker.o: input_maker.cc input_maker.h string+.o ftran_structs.o string+.o button_manager.o helpers.o
	g++ $(C_FLG) -c input_maker.cc

fop_handler.o: fop_handler.cc fop_handler.h hf_qvalue deck.o
	g++ $(C_FLG) -c fop_handler.cc

hf_qvalue:
	make -C ./q_val/

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

helpers.o: logger.o helpers.h helpers.cc
	g++ $(C_FLG) -c helpers.cc

#runs valgrind on the debug executable created by make gdb
#do this to check for memory leaks and double free errors
valgrind:
	valgrind $(vtool) $(vopt) ./debug

#compile for debugging, or when includes have gotten messed up
gdb:
	g++ -g -o debug $(C_FLG) *.cc $(SDL_INC) $(F_OBJS)

#pack useful files up for an email or storage
tar:
	tar -czvf andiamo.tar.gz *.cc *.h Makefile doxyfile readme.md tile_Input Assets sandbox config sandbox output

#have doxygen run and create a manual from source comments and the configuration settings in doxyfile
doxy: doxyfile
	doxygen doxyfile
	$(browser) ./doxyout/html/index.html

#remove compiled things, text editor saves, memory check output
clean:
	make clean -C ./q_val/ && rm *~ *.o *.h.gch andiamo debug memory_check.txt




