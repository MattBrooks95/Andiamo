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

class form{

  public:
	form();
	~form();


	void init(std::string form_title_in,std::string help_msg_image_name,unsigned int num_of_pages_in,
		  int xloc_in,int yloc_in, sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);


	virtual void form_event_loop(SDL_Event& big_event);


	void handle_click(SDL_Event& mouse_event,bool& done,bool& click_lock);

	void draw_me();

	void toggle_active();

	bool is_active(){ return active;}


	void next_page();

	void prev_page();


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
	//! page constructor allows for the setting of important variables
	/*! \param num_columns_in is the # of columns of text boxes to create
	*\param num_rows_in is the # of rows of text boxes to create
	*\param row_labels_in is the row labels, which is not always used
	*\param column_labels_in is the column labels, which should almost always be specified */
	page(unsigned int num_columns_in, unsigned int num_rows_in,const std::vector<std::string>& column_labels_in,
	     std::vector<std::string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);
	~page();
	//! this function draws the pages headers, labels and text boxes
	void draw_me();

	//! this function accepts three structs which specify what type of parameters are needed on the page
	/*! The structs contain the number of each kind of parameter needed, and also the column number
	 *that will be assigned to that type. It has been overloaded, because not all of the types will be used */
	void set_parameters(const std::vector<std::string>& int4_list,const std::vector<std::string>& real8_list,
			    const std::vector<std::string>& string_list);
	void set_parameters(const std::vector<std::string>& int4_list,const std::vector<std::string>& real8_list);	
	void set_parameters(const std::vector<std::string>& int4_list);


  private:
	sdl_help* sdl_helper;//!< allows easy access to the graphics class
	TTF_Font* sdl_font;//!< allows easy access to the text style used in other constructs

	unsigned int num_columns;//!< number of columns that will be needed
	unsigned int num_rows;//!< number of rows that will be needed

	std::vector<std::string> column_labels;//!< column labels that populate the top of the page
	std::vector<std::string> row_labels;//!< optional row labels, should be rarely used
	
	std::vector<text_box> text_boxes;//!< array that contains all of the necessary text boxes
};
