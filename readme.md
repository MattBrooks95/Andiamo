	Welcome to Andiamo's Manual! Andiamo is the graphical front end to Ohio University's Hauser-
Feshbach fomula implementing HF???? fortran code.

Andiamo allows for users to graphically enter inputs to be used in the HF???? fortran code, as 
opposed to preparing an input file beforehand.

The graphics component is a vector that can hold various customizable field objects that act like 
'cards' or 'tiles' whose variable storage and look is detailed by the tiles.txt input file. This input file 
has a particular format that matches the format in the file "example.txt." If the project directory hasn't 
been changed at all, this file can be found at "proj_dir/tile_Input/example.txt"

We're not yet sure what version of the HF fortran code we're going to use yet. We are also not sure if
andiamo will run that code "in place" using c++ -> fortran bindings, or if it will generate and input file
and then run HF???? on that generated file using system calls.

Compatability wise, c++11 and gcc 4.9 or greater is required to build this source. I made this decision
because <regex> is very handy for processing the text files. I'm not yet quite sure how developing for 
Windows and Linux simultaneously is done, but I'd eventually like it to be able to run
on both. We are developing for Linux for now.
