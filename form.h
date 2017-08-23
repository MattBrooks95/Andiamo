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
class page;

class form{

  public:
	form();
	~form();


	void form_init(std::string form_title_in,unsigned int num_of_pages_in,SDL_Rect form_area_in,
		       sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);


	void draw_me();

	void toggle_active();

	void next_page();

	void prev_page();


  private:

	std::string form_title;
	SDL_Surface* form_title_surface;
	SDL_Texture* form_title_texture;

	SDL_Surface* form_surface;
	SDL_Texture* form_texture;


	SDL_Rect form_area;

	bool active;

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
	page(unsigned int num_columns_in, unsigned int num_rows_in,std::vector<std::string>& column_labels_in,
	     std::vector<std::string>& row_labels_in,sdl_help* sdl_helper_in);
	~page();

  private:
	sdl_help* sdl_helper;


	unsigned int num_columns;
	unsigned int num_rows;

	std::vector<std::string> column_labels;
	std::vector<std::string> row_labels;
	
};
