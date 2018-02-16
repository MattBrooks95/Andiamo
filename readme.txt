	Welcome to Andiamo's Manual! Andiamo is the graphical front end to Ohio
University's Hauser-Feshbach fomula implementing fortran code.


	I have beenlearning graphics programming as I go. The code also has many
printing functions that aren't meant for final product use, but rather
for my own debugging purposes.

	Andiamo allows for users to graphically enter inputs to be used in the
Fortran code, as opposed to preparing an input file in a text editor.

	A detailed class diagram explaining how the different objects in this
program work together, and what they do, will be created very shortly. I've
recently re-structured the software to separate the parameter entry and
graphics as much as possible, so each module is specialized and easy
to understand.

	The Hauser-Feshbach (HF) code is ran using system calls. Andiamo
uses user input and a tool called Frank's Optical Potentials to create
the input file for the HF tool.

	Compatability wise, c++11 and gcc 4.9 or greater is required to build
Andiamo. I made this decision because <regex> is very handy for processing
text files, and doing integrity checks on values inputted by the user.

	We are currently only targeting a Linux environment for Andiamo,
as directory parsing under Windows is something I haven't learned yet.
Hopefully, I can look into Windows once we get Andiamo running and tested.

############################################################################


The tools in use thus far are:
	1)SDL2 https://www.libsdl.org/
		a) SDL_Image https://www.libsdl.org/projects/SDL_image/
		b) SDL_TTF https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html
	2) Doxygen http://www.stack.nl/~dimitri/doxygen/
	3) Valgrind http://valgrind.org/

	If these tributes are not to the liking of these tool's respective
creators please email me about how I can go about fixing it.

Matt Brooks
mb577513@ohio.edu


