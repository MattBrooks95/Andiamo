/*! \file form_buttons.h describes special buttons that are activated by parameters in the tile fields, and allows for
 * extensive text dynamic text entry, as required by some of the parameters specified in the input manual */

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

//! "base" class for form buttons, that provides their general form, and can be inherited from to make more specialized forms
class form_button : public button{

  public:
	//! the constructor NULLs all of the fields
	 form_button();
	//! the destructor returns memory back to the operating system when the object is killed
	~form_button();


	//! this function is an overload of the virtual init function from the default button class
	/*! these buttons are drawn right on the form tray, so they don't need to hold an image */
	void init(sdl_help* sdl_help_in);


	//! this function is used by the button manager to make sure this button lines up with its image in the form tray
	void set_corner_loc(int x_in, int y_in);
	//! this function is used by the button manager to set width and height
	/*! though 100x50 is the default dimensions of each of the form buttons */
	void make_rect(int width_in = 100,int height_in = 70);

	//! this function sets up the lock's location
	virtual void setup_lock();
	//! this function can be overloaded or used by derived classes
	/*! overloading it allows a specific picture file to be used as the help dialogue when the user
	 *clicks on the form while it is still locked, and explains its purpose and unlocking conditions */ 
	virtual void setup_help_msg();

	//! this function initializes this buttons form object
	/*! It must be overloaded by the derived form_buttons to have any meaning */
	virtual void init_form(const vector<regex>& pattern_tests);

	//! this function resizes the window if it is too small when the user tries to open a form
	virtual void screen_size();

	//! this function can be overloaded or used by derived classes
	/*! if a special message box is made, it would be wise to overload this as well, so it can be placed
	 * in a different location */
	virtual void draw_help_msg(SDL_Event& big_event, SDL_Rect& destination);

	//! This member draws the lock to the screen, if the button is in lock mode
	void draw_lock();

	//! this member changes it's modes from locked to unlocked, and the reverse
	void toggle_lock();

	//! this member is a getter for the is_locked boolean
	const bool& get_is_locked(){ return is_locked;}

	//! this member has the form objects contained herein send their information to the input_maker
	/*! this is currently a stub */
	virtual bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	virtual bool check_values(vector<index_value>& error_details);

	//! this function is a const getter by reference for this button's form
	const form& get_form(){ return my_form;}

  protected:
	SDL_Texture* lock_texture;//!< save the texture for the lock, when this button is not in use
	SDL_Rect lock_rect;//!< save location & size of the lock

	bool is_locked;//!< control whether or not to show the lock, and prevent the button from being used

	form my_form;//!< object which allows for dynamic parameter entry

	SDL_Texture* unlock_help_texture;
};

class icntrl8_form_button : public form_button{

  public:

		//! this function changes the default help message to one that explains icntrl8's conditions
	void setup_help_msg();

	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! sets up a form that suits the needs of icntrl8's logics per the input manual
	void init_form(const vector<regex>& pattern_tests);

	//! this function opens the icntrl8 form on click
	void click_helper(SDL_Event& mouse_event);

	//! this function is a helper that compacts the code found in click_helper that creates or recreates pages
	void page_creation_helper();

	//! this function outputs the form's info to the input_maker's file stream when called
	bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:
	unsigned int icntrl8_val;//!< updated with the value from the field that corresponds to Cutoff Nuclei

};

class icntrl6_form_button : public form_button{

  public:
	//! this destructor frees the memory unique to this form button
	/*! the language calls ~form_button() automatically */
	~icntrl6_form_button();

	//! setup_lock is overloaded here, because it looks better in the lower right corner
	void setup_lock();

	//! sets up a form that suits the needs of icntrl6's logics per the input manual
	void init_form(const vector<regex>& pattern_tests);

	//! this function sets up a reference to the button manager
	/* this object needs reference to the button_manager because it needs to be able to draw the
	 *button tray in it's form selection loop */
	void set_bmanager_reference(button_manager* b_manager_in) { b_manager = b_manager_in; }


	//################ PAGE CREATION HELPERS ##################################################
	//! this member sets up the parity form's pages
	void parity_page_creation();
	//! this fills up the row labels for the parity form
	void fill_parity_labels(vector<string>& row_labels,vector<string>& column_labels);

	//! this member sets up the search spectra form's pages
	void search_spectra_page_creation();
	//! this helper member fills in the column labels for the search spectra form
	void fill_spectra_vectors(vector<string>& pass_column_labels,vector<int>& column_spaces);
	//! This helper abstracts some code to make search_spectra_page_creation more readable
	void search_spectra_page_helper();

	//! this member sets up the xsection form's pages
	void cross_sections_page_creation();
	//! this helper abstracts some code to make cross_sections_page_creation more readable
	void cross_sections_helper();
	//#########################################################################################


	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl6 form on click
	void click_helper(SDL_Event& mouse_event);

	//! this function sets up the form selection page
	void setup_landing();

    //! this function updates the loading screen in response to a screen resize
    void update_landing();

	//! this function shows the image that allows the user to switch between icntrl6's forms
	void show_landing();

	//! does the logic for figuring out if a mousebutton down event clicked somewhere within form selection
	bool landing_was_clicked(SDL_Event& mouse_event);

	//! this sends this object's info the input_manager's file stream
	bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:

	int INM1_val;//!< used to keep track of what conditions caused the current pages to be made
	int INM2_val;//!< used to keep track of what conditions caused the current pages to be made

	button_manager* b_manager;//!< pointer to the button manager, to call it's drawing functions

	SDL_Texture* landing_texture;//!< saves texture for the form selection image

	SDL_Rect landing_rect;//!< saves the location info for the form selection feature

	active_area parity_area;//!< controls clicking logics to open parity_area form
	active_area spectra_area;//!< controls clicking logics to open spectra_area form
	active_area xsections_area;//!< controls clicking logics to open xsections_area form

	form search_spectra;//!< extra form # 1, for making INM1 line in input manual
	form cross_sections;//!< extra form # 2 for making INM2 line in input manual

};

class icntrl10_form_button : public form_button{

  public:

	//! impelements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! sets up a form that suits the needs of icntrl10's logics per the input manual
	void init_form(const vector<regex>& pattern_tests);


	//! this function opens the icntrl10 form on click
	void click_helper(SDL_Event& mouse_event);

	//!  outputs form info to the input_maker file stream
	bool make_output(ofstream& outs, vector<index_value>& bad_input_list);

  private:
};

class icntrl4_form_button : public form_button{

  public:
	//! setup_lock is overloaded here, because it looks better on this form button in the lower right corner
	void setup_lock();

	//! sets up a form that suits the needs of icntrl4's logics per the input manual
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

    //! not called icntrl4_val because derivative parameter nch4 controls # of text boxes needed
    unsigned int nch4_val;
};

class ilv3_ilv5_form_button : public form_button{

  public:

	//! sets up a form that suits the needs of icntrl4's logics per the input manual
	void init_form(const vector<regex>& pattern_tests);

	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl4 form on click
	void click_helper(SDL_Event& mouse_event);

    //! helper function for click_helper that sets up pages
    void page_creation_helper();

    //! this function is invoked by the input_maker to have this form output to the HF file
    bool make_output(ofstream& outs,vector<index_value>& bad_input_list);

	//! check to make sure the inputs are properly formed
	bool check_values(vector<index_value>& error_details);

  private:
        
};





















