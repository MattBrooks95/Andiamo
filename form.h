//! \file form.h contains the descriptions of the form class, and also the page class

//! \class form describes the text entry forms used for the dynamic entry of parameters as a result of other parameters enabling extra lines
/*! to be used by the form_buttons class */
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include<ostream>
#include<vector>
#include<string>

#include "button.h"
#include "text_box.h"
class page;

//! A form is a class that is housed by a form_button, which allows for dynamic parameter entry
/*! Forms are only used for the handful of parametes in the input manual that enable extra features of
 *HF, which typically require a significant amount of extra input. Each form is a collection of page objects,
 *and the various functions that are useful to its implementation. I've tried to create it to be as dynamic as possible,
 *though inheriting from this class to create even more specialized functionality is possible */
class form{

  public:
	//! the default constructor, which takes no arguments, and zeroes or NULLs the form's values
	form();
	//! the destructor gets rid of the form's textures, giving that memory back to the OS
	~form();

	//! the init function sets up the form's variables as specified by the parameters it is passed.
	/*! \param form_title_in sets the title that is displayed at the top of the form
	 *\param help_msg_image_name finishes the asset's path, by specifying the help message png to be used
	 *\param xloc_in sets the horizontal coordinate of the corner of the form, likely should be 0
	 *\param yloc_in sets the vertical coordinate of the corner of the form, likely should be 0
	 *\param sdl_helper_in is a pointer reference to the sdl_help object, for drawing functionality
	 *\param sdl_font_in is a pointer to the font contained within the sdl_help object, for shorthand access */
	void init(std::string form_title_in,std::string help_msg_image_name,
		  int xloc_in,int yloc_in, sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);

	//! form_event_loop is a member that enacts an event loop similar to the one in main
	/* it is specifically tailored to be useful for form parameter entry, and it is virtual so it could be
	 *inherited by narrow forms to allow specialized functionality */
	virtual void form_event_loop(SDL_Event& big_event);

	//! handle click the is the function that processes clicks and enacts the according object's work functions
	void handle_click(SDL_Event& mouse_event,bool& done,bool& click_lock);

	//! draw_me presents the form to the screen, which involves printing its currently active page as well
	void draw_me();

	//! toggle_active() is used to flip the active boolean
	/*! the active boolean tells the application when the form should be visible & interactable, and when
	 * it should not be */
	void toggle_active();

	//! is_active() is a by-value getter for the active boolean
	bool is_active(){ return active;}

	//! next_page presents the next page to the user, if possible
	void next_page();

	//! prev_page presents the previous page to the user, if possible
	void prev_page();

	//! getter for the vector that stores the pages of text boxes
	std::vector<page>& get_pages() { return pages;} 

	//! this funciton sets page_count to the passed value
	void set_page_count(int page_count_in);
	//! this function updates the sprites that represent which page the user is currently on
	/*! these are the numbers at the top right of the form, next to the page left and right buttons */
	void update_page_indicator();
	//! this function destroys the pages that were previously created, so they can be recreated
	void flush_pages();

	//! this is the text editing loop for the text boxes within a page
	/*! \param_current_box supplies function a reference to which text box is being edited
	 *\param event is a reference to the sdl event containeer
	 *\param command is a string that may be changed and returned from the functon, to tell the form_event_loop
	 *if any necessary behaviours are needed, like tabbing to the next text box */
	void text_box_loop(text_box& current_box,SDL_Event& event,std::string& command);


	bool prev_initiated;
	int prev_init_value;

  private:

	std::string form_title;
	SDL_Surface* form_title_surface;

	SDL_Surface* form_surface;
	SDL_Texture* form_texture;

	SDL_Surface* help_surface;
	SDL_Texture* help_texture;

	//! this is a sprite sheet with numbers 0-9 on it, so parts can be used to show page numbers
	SDL_Surface* number_sprites;

	SDL_Rect form_area;

	bool active;

	bool help_shown;

	active_area exit;
	active_area help;
	active_area right_arrow;
	active_area left_arrow;

	std::vector<page> pages;
	unsigned int page_count;
	unsigned int current_page;


	sdl_help* sdl_helper;
	TTF_Font* sdl_font;

};

class page{

  public:

	//! default page constructor
	page();


	//! page constructor allows for the setting of important variables
	/*! \param num_columns_in is the # of columns of text boxes to create
	*\param num_rows_in is the # of rows of text boxes to create
	*\param row_labels_in is the row labels, which is not always used
	*\param column_labels_in is the column labels, which should almost always be specified */
/*
	page(unsigned int num_columns_in, unsigned int num_rows_in,const std::vector<std::string>& column_labels_in,
	     std::vector<std::string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);
*/
	
	//! writing the copy constructer here prevents seg faults due to double free when pushed into a vector
	page(const page& other);

	//! destructor frees memory back to the OS, most importantly taking care of pointers to memory
	~page();

	//! this function is used when the detailed constructor can't be ran
	/*
	void page_init(unsigned int num_columns_in, unsigned int num_rows_in,const std::vector<std::string>& column_labels_in,
	     std::vector<std::string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,int additional_spacing);
	*/

	void page_init(unsigned int num_columns_in, unsigned int rows_needed,
		       const std::vector<std::string>& column_labels_in, std::vector<std::string>& row_labels_in,
		       sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,int additional_spacing);


	//! this function draws the pages headers, labels and text boxes
	void draw_me();

	//! this function sets up the logic that keeps track of which column of rows are which fortran type
	void column_logic(const std::vector<std::string>& types);

	//! this function is a non-const reference getter for the text box vector
	std::vector<text_box>& get_text_boxes(){ return text_boxes;}
	//! this function is a const reference getter for the text box vector
	const std::vector<text_box>& get_const_text_boxes(){ return text_boxes;}

	//! this function is a getter for the # of columns on the page
	unsigned int get_columns(){ return num_columns;}
	//! this function is a getter for the # of rows on the page
	unsigned int get_rows(){ return num_rows;}

  private:
	sdl_help* sdl_helper;//!< allows easy access to the graphics class
	TTF_Font* sdl_font;//!< allows easy access to the text style used in other constructs

	unsigned int num_columns;//!< number of columns that will be needed
	unsigned int num_rows;//!< number of rows that will be needed

	std::vector<std::string> column_labels;//!< column labels that populate the top of the page
	std::vector<std::string> row_labels;//!< optional row labels, should be rarely used
	
	std::vector<text_box> text_boxes;//!< array that contains all of the necessary text boxes
	std::vector<SDL_Texture*> column_label_textures;//!< stores the column label textures for the passed labels
	std::vector<SDL_Rect> column_label_rects;//!< store the drawing location for the column labels

	std::vector<SDL_Texture*> row_label_textures;//!< store textures for the row labels
	std::vector<SDL_Rect> row_label_rects;//!< store the location of the textures for the row labels
};





