/*! \brief \file form_buttons.h describes special buttons that are activated by
 *parameters in the tile fields */
/*! \details \file form_buttons.h allows for extensive text
 *dynamic text entry, as required by some of the parameters
 *specified in the input manual */

#pragma once
#include<fstream>
#include "button.h"
#include "form.h"
#include "c_tuples.h"
#include "asset_manager.h"

using std::string;
using std::vector;
using std::regex;
using std::ofstream;

/*! \brief "base" class for form buttons, that provides their general form,
 *and can be inherited from to make more specialized forms */
class form_button : public button{

  public:
	//! the constructor NULLs all of the fields
	 form_button();
	//! returns memory back to the operating system when the object is killed
	~form_button();


	//! overload of the virtual init function from the default button class
	/*! these buttons are drawn right on the form tray,
	 *so they don't need to hold an image */
	void init();


	/*! used by the button manager to make sure this button lines
	 *up with its image in the form tray */
	void set_corner_loc(int x_in, int y_in);

	//! this function is used by the button manager to set width and height
	/*! though 100x50 is the default dimensions of each of the form buttons */
	void make_rect(int width_in = 100,int height_in = 70);

	//! this function sets up the lock's location
	virtual void setup_lock();

	//! this function can be overloaded or used by derived classes
	/*! overloading it allows a specific picture file to be used as the
	 *help dialogue when the user clicks on the form while it is
	 *still locked, and explains its purpose and unlocking conditions */ 
	virtual void setup_help_msg();

	//! this function initializes this buttons form object
	/*! must be overloaded by the derived form_buttons to have any meaning */
	virtual void init_form(const vector<regex>& pattern_tests);

	/*! this function resizes the window if it is too small when
	 *the user tries to open a form */
	virtual void screen_size();

	//! this function can be overloaded or used by derived classes
	/*! if a special message box is made, it would be wise to overload
	 *this as well, so it can be placed in a different location */
	virtual void draw_help_msg(SDL_Event& big_event, SDL_Rect& destination);

	//! This member draws the lock to the screen, if the button is in lock mode
	void draw_lock();

	//! changes it's modes from locked to unlocked, and the reverse
	void toggle_lock();

	//! is a getter for the is_locked boolean
	const bool& get_is_locked(){ return is_locked;}

	/*! has the form objects contained herein send their information
	 *to the input_maker */
	virtual bool make_output(ofstream& outs,
								vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	virtual bool check_values(vector<index_value>& error_details);

	//! this function is a const getter by reference for this button's form
	const form& get_form(){ return my_form;}

  protected:
	//! save the texture for the lock, when this button is not in use
	SDL_Texture* lock_texture;

	//! save location & size of the lock
	SDL_Rect lock_rect;

	/*! control whether or not to show the lock,
	 *and prevent the button from being used */
	bool is_locked;

	//! object which allows for dynamic parameter entry
	form my_form;

	SDL_Texture* unlock_help_texture;
};

//! opens the form "Cutoff Nuclei"
class icntrl8_form_button : public form_button{

  public:

	/*! this function changes the default help message to one that
	 *explains icntrl8's conditions */
	void setup_help_msg();

	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	/*! sets up a form that suits the needs of icntrl8's logics
	 *per the input manual */
	void init_form(const vector<regex>& pattern_tests);

	//! this function opens the icntrl8 form on click
	void click_helper(SDL_Event& mouse_event);

	/*! this function is a helper that compacts the code found
	 *in click_helper that creates or recreates pages */
	void page_creation_helper();

	/*! this function outputs the form's info to the input_maker's
	 *file stream when called */
	bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:
	//! updated with the value from the field that corresponds to Cutoff Nuclei
	unsigned int icntrl8_val;

};

//! opens a form based off line 10's ITER, INM1 & INM2 values, for search
class icntrl6_form_button : public form_button{

  public:
	//! this destructor frees the memory unique to this form button
	/*! the language calls ~form_button() automatically */
	~icntrl6_form_button();

	//! setup_lock is overloaded here, it looks better in the lower right corner
	void setup_lock();

	//! sets up a form for icntrl6's logics per the input manual
	void init_form(const vector<regex>& pattern_tests);

	//############## PAGE CREATION HELPERS ###################################
	//! this member sets up the parity form's pages
	void parity_page_creation();

	//! this fills up the row labels for the parity form
	void fill_parity_labels(vector<string>& row_labels,
							vector<string>& column_labels);

	//! this member sets up the search spectra form's pages
	void search_spectra_page_creation();

	//! this helper fills in the column labels for the search spectra form
	void fill_spectra_vectors(vector<string>& pass_column_labels,
								vector<int>& column_spaces);

	/*! This helper abstracts some code to make search_spectra_page_creation
	 * more readable */
	void search_spectra_page_helper();

	//! this member sets up the xsection form's pages
	void cross_sections_page_creation();

	/*! this helper abstracts some code to make cross_sections_page_creation
	 *more readable */
	void cross_sections_helper();
	//########################################################################


	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl6 form on click
	void click_helper(SDL_Event& mouse_event);

	//! this function sets up the form selection page
	void setup_landing();

    //! this function updates the loading screen in response to a screen resize
    void update_landing();

	//! shows the image that allows the user to switch between icntrl6's forms
	void show_landing();

	/*! figures out if a mousebutton down event clicked somewhere
	 *within form selection */
	bool landing_was_clicked(SDL_Event& mouse_event);

	//! this sends this object's info the input_manager's file stream
	bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:

	//! track of what conditions caused the current pages to be made
	int INM1_val;

	//! track of what conditions caused the current pages to be made
	int INM2_val;

	//! saves texture for the form selection image
	SDL_Texture* landing_texture;

	//! saves the location info for the form selection feature
	SDL_Rect landing_rect;

	//! controls clicking logics to open parity_area form
	active_area parity_area;

	//! controls clicking logics to open spectra_area form
	active_area spectra_area;

	//! controls clicking logics to open xsections_area form
	active_area xsections_area;

	//! extra form # 1, for making INM1 line in input manual
	form search_spectra;

	//! extra form # 2 for making INM2 line in input manual
	form cross_sections;

};

//! special object for icntrl10_button
class icntrl10_data{

  public:
    icntrl10_data(){
        line_entries.resize(3);
    }

    vector<text_box> line_entries;


  private:


};


//! opens the form "Spin Cutoff Info"
/*! this form hasn't been implemented yet, because it is a
 *special case */
class icntrl10_button : public button{

  public:

    icntrl10_button();

    //! does the same logic as the form_button::init
    void init();

    //! set where this form button should be on the button tray
    void set_corner_loc(int x, int y);

    //! sets up the sdl rect
    void make_rect(int width_in = 100,int height_in = 70);

    //! set up the lock graphic for when icntrl10's data entry isn't active
    void setup_lock();

    //! draws the lock if icntrl10 isn't activated
    void draw_lock();

    //! changes the lock state of the icntrl10 button
    void toggle_lock();

    //! explains why the data entry is locked, when the button is clicked
    void draw_help_msg(SDL_Event& big_event,SDL_Rect& destination);

	//! impelements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl10 form on click
	void click_helper(SDL_Event& mouse_event);

    //! event loop for entering in icntrl10 info
    void event_loop(SDL_Event& mouse_event);

    //! figure out which text box the user clicked on, and allow editing
    void event_loop_click(SDL_Event& mouse_event,bool& done,bool& click_lock);

    //! change current context from n in NNSIG to n + 1
    void page_right();

    //! change current context from n in NNSIG to n - 1
    void page_left();

    //! changes the number for the current page in the top right
    void update_page_indicator();

    //! process what the user types for the passed text box
    void text_entry(text_box& curr_tb,SDL_Event& event,bool& done,
                    string& command,unsigned int which_box);

    //! presents the entry form to the user
    void draw_me();


	//!  outputs form info to the input_maker file stream
	bool make_output(ofstream& outs);

    //! prepares the linear parameter entry 
    void init_data(unsigned int num_contexts);

    //! getter for locked status
    bool get_is_locked(){ return is_locked;}

    //! activate or de-activate this entry method
    void toggle_active(){ active = !active; }

    //! save placement for the lock graphic
    SDL_Rect lock_rect;

    //! stores the clickable location for the exit button in the top left
	active_area exit;

    //! stores the clickable location for the page right arrow in the top right
	active_area right_arrow;

    //! stores the clickable location for the page left arrow in the top right
	active_area left_arrow;

    //! template for the icntrl10 graphical entry
    SDL_Texture* icntrl10_backdrop;

    //! surface that the page #s and text boxes should be blitted onto
    SDL_Surface* over_surface;

    //! for drawing overlay (page numbers, text box) to the screen
    SDL_Texture* over_texture;

  private:

	/*! sprite sheet with numbers 0-9 on it,
	 * so parts can be used to show page numbers */
	SDL_Surface* number_sprites;

    //! keep track of where to draw the backdrop
    SDL_Rect bd_dest;

    //! store the graphical representation of the "lock"
    SDL_Texture* lock_texture;

    SDL_Texture* unlock_help_texture;

    //! boolean to store whether or not this data entry is accessible
    bool is_locked;

    //! stores the objects for NNSIG lines of ICNTRL10 information
    vector<icntrl10_data> data;

    //! keep track of the previous NNSIG value that started this form
    unsigned int prev_NNSIG;

    //! keep track of the index of icntrl10 info being modified
    /*! the current one should be the only one shown on the screen */
    int current_context;

    //! true when the user is interacting with this object, false elsewise
    bool active;

    //! store the 3 patterns necesary for checking these lines
    vector<regex> my_patterns;
};

//! opens the form "Resolved Levels"
class icntrl4_form_button : public form_button{

  public:
	/*! setup_lock is overloaded here, because it looks better on this
	 *form button in the lower right corner */
	void setup_lock();

	//! sets up a form that suits the needs of icntrl4's logics
	void init_form(const vector<regex>& pattern_tests);

    //! helper function for form creation
    void page_creation_helper();

	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl4 form on click
	void click_helper(SDL_Event& mouse_event);

    //! outputs form info to the input_maker file stream
    bool make_output(ostream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:

    /*! not called icntrl4_val because derivative parameter
	 *nch4 controls # of text boxes needed */
    unsigned int nch4_val;
};

//! opens the ILV3 or ILV5 form
/*! the form that is created depends on whether parameter ILV3 or
 *parameter ILV5 was used to unlock this button */
class ilv3_ilv5_form_button : public form_button{

  public:

	//! sets up a form that suits the needs of icntrl4's logics
	void init_form(const vector<regex>& pattern_tests);

	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl4 form on click
	void click_helper(SDL_Event& mouse_event);

    //! helper function for click_helper that sets up pages
    void page_creation_helper();

    //! invoked by the input_maker to have this form output to the HF file
    bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:
        
};







