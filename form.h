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
#include "c_tuples.h"
#include "asset_manager.h"


using std::string;
using std::vector;
using std::regex;

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
	 *\param yloc_in sets the vertical coordinate of the corner of the form, likely should be 0 */
	void init(string form_title_in,string help_msg_image_name, int xloc_in,int yloc_in,
				const vector<regex>& pattern_tests);

    //! this function changes the form's title
    /*! this is useful, for example, when a form can serve one or two logical functions
     *but the required information is the same. The ilv3/ilv5  form is one such case. Make sure to
     *account for the form title area's background color, as that color is needed to fill over the old
     *title, so the new one can be applied. */
    void set_form_title(string new_title);

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

	//! this function checks all of the inputs to make sure they are correct
	bool check_values(vector<index_value>& error_details);

	//! getter for the vector that stores the pages of text boxes
	vector<page>& get_pages() { return pages;} 

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
     *\param pattern is the regex used to test user input as they type
	 *if any necessary behaviours are needed, like tabbing to the next text box */
	void text_box_loop(text_box& current_box,SDL_Event& event,string& command,
                       const regex& pattern);


    //these two functions are used to know when a form needs smashed and recreated
    //because the user specified another size
	bool prev_initialized;  //!< remember if this form has been initialized or not
	int prev_init_value;  //!< keep track of the value this form was initialized with

    //! title of the form, which is displayed at the top of the form
	string form_title;

  private:

    //! save the title surface so it can be blitted to the form's surface
	SDL_Surface* form_title_surface;

    //! save the form's surface
	SDL_Surface* form_surface;
    //! save the form's texture
	SDL_Texture* form_texture;

    //! save the help page's texture
	SDL_Texture* help_texture;

	//! this is a sprite sheet with numbers 0-9 on it, so parts can be used to show page numbers
	SDL_Surface* number_sprites;

    //! keep track of the form's dimensions and location
	SDL_Rect form_area;

    //! keep track of whether or not this form is active
    /*! active means that it should be drawn to the screen, and it should
     *check mouse clicks upon itself. This being false allows the form's state to be saved,
     *but not visible to the user or interactable until it is reopened. */
	bool active;

    //! keep track of whether to show the page's texture or the help page's
	bool help_shown;

    //! keep track of the clickable location for the exit button in the top left
	active_area exit;

    //! keep track of the clickable location for the help arrow in the top left 
	active_area help;

    //! keep track of the clickable location for the page right arrow in the top right
	active_area right_arrow;

    //! keep track of the clickable locatino for the page left arrow in the top right
	active_area left_arrow;

    //! store vector of pages, which each contain an array of text boxes for the user to fill in
	vector<page> pages;

    //! keep track of how many pages there are, so it can be displayed in the top right
	int page_count;

    //! keep track of what page the user is currently interacting with
    /*! this will be shown in the top right, and will change as the user hits the
     *right and left arrows */
	int current_page;

    //! store the regular expressions that can be used for real time error checking
    /*! kind of like a word processor, I want the text box to indicate if an invalid
     *input has been typed */
	vector<regex> my_patterns;

    //! pointer to the graphics class
	sdl_help* sdl_helper;
    //! pointer to the font in use in the graphic's class
	TTF_Font* sdl_font;

};

//! class page describes a class that is housed within a forms class, and handles the text
/*! it handles the text boxes and the graphical representation of itself. Form's have a vector
 *of these, so only the one being edited is displayed and interactable */
class page{

  public:

	//! default page constructor
	page();


	//! page constructor allows for the setting of important variables
	/*! \param num_columns_in is the # of columns of text boxes to create
	*\param num_rows_in is the # of rows of text boxes to create
	*\param row_labels_in is the row labels, which is not always used
	*\param column_labels_in is the column labels, which should almost always be specified */
	
	//! writing the copy constructer here prevents seg faults due to double free when pushed into a vector
	page(const page& other);

	//! destructor frees memory back to the OS, most importantly taking care of pointers to memory
	~page();

	//###########################################################################################################
	void page_init(unsigned int num_columns_in, unsigned int rows_needed,
		       const vector<string>& column_labels_in,vector<string>& row_labels_in,
		       const vector<int>& column_spacings);
	//! helper for page_init, does the mundane pass-through assignments
	void page_init_local_var(unsigned int num_columns_in, unsigned int rows_needed, const vector<string>& column_labels_in,
			         vector<string>& row_labels_in);
	//! helper for page_init that sets up the row labels, if they exist
	void page_init_set_row_labels(const vector<string>& row_labels_in,int& x_start_point);

	//! helper for page_init that sets up the text boxes
	void page_init_set_text_boxes(int& x_start_point,const vector<int>& column_spacings,bool& row_labels_exist);

	//! helper for page_init that sets up the column labels/headers
	void page_init_column_labels(const vector<int>& column_spacings,int& x_start_point,bool& row_labels_exist);
	//###########################################################################################################

	//! this function draws the pages headers, labels and text boxes
	void draw_me();

	//! this function is a non-const reference getter for the text box vector
	vector<text_box>& get_text_boxes(){ return text_boxes;}

	//! this function is a const reference getter for the text box vector
	const vector<text_box>& get_const_text_boxes(){ return text_boxes;}

	//! this function returns a reference to the row labels vector
	/*! this is the const version */
	const vector<string>& get_row_labels(){
		return row_labels;
	}
	//! return modifying reference to row labels vector
	vector<string>& get_const_row_labels(){
		return row_labels;
	}
	

	//! this function is a getter for the # of columns on the page
	unsigned int get_columns(){ return num_columns;}

	//! this function is a getter for the # of rows on the page
	unsigned int get_rows(){ return num_rows;}

  private:

	unsigned int num_columns;//!< number of columns that will be needed
	unsigned int num_rows;//!< number of rows that will be needed

	//! column labels that populate the top of the page
	vector<string> column_labels;

	//! optional row labels, should be rarely used
	vector<string> row_labels;
	
	//! array that contains all of the necessary text boxes
	vector<text_box> text_boxes;

	//! stores the column label textures for the passed labels
	vector<SDL_Texture*> column_label_textures;

	//! store the drawing location for the column labels
	vector<SDL_Rect> column_label_rects;

	//! store textures for the row labels
	vector<SDL_Texture*> row_label_textures;

	//!< store the location of the textures for the row labels
	vector<SDL_Rect> row_label_rects;
};









