/*! \file form_buttons.h describes special buttons that are activated by parameters in the tile fields, and allows for
 * extensive text dynamic text entry, as required by some of the parameters specified in the input manual */

#pragma once

#include "button.h"
#include "form.h"
//! "base" class for form buttons, that provides their general form, and can be inherited from to make more specialized forms
class form_button : public button{

  public:
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

	//! This member draws the lock to the screen, if the button is in lock mode
	void draw_lock();

	//! this member changes it's modes from locked to unlocked, and the reverse
	void toggle_lock();

	//! this member is a getter for the is_locked boolean
	const bool& get_is_locked(){ return is_locked;}
  protected:
	SDL_Surface* lock_surface;//!< save the surface for the lock, when this button is not in use
	SDL_Texture* lock_texture;//!< save the texture for the lock, when this button is not in use
	SDL_Rect lock_rect;//!< save location & size of the lock

	bool is_locked;//!< control whether or not to show the lock, and prevent the button from being used

};

class icntrl8_form_button : public form_button{

  public:
	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);


	//! this function opens the icntrl8 form on click
	void click_helper(SDL_Event& mouse_event);

  private:
	

};

class ilv2_form_button : public form_button{

  public:
	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the ilv2 form on click
	void click_helper(SDL_Event& mouse_event);


  private:

};

class icntrl6_form_button : public form_button{

  public:
	//! setup_lock is overloaded here, because it looks better in the lower right corner
	void setup_lock();


	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl6 form on click
	void click_helper(SDL_Event& mouse_event);

  private:

};

class icntrl10_form_button : public form_button{

  public:
	//! impelements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl10 form on click
	void click_helper(SDL_Event& mouse_event);

  private:

};

class icntrl4_form_button : public form_button{

  public:
	//! setup_lock is overloaded here, because it looks better on this form button in the lower right corner
	void setup_lock();


	//! implements the special logic for this class
	bool handle_click(SDL_Event& mouse_event);

	//! this function opens the icntrl4 form on click
	void click_helper(SDL_Event& mouse_event);

  private:

};































