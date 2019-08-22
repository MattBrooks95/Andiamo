//! describes a page of text boxes that are dynamically instantiated based on user entered values
#pragma once

#include <vector>
#include <string>

#include "text_box.h"

#include <SDL2/SDL.h>

//! default text box width for form
#define TEXT_BOX_W 60
//! default horizontal distance between text boxes
#define TEXT_BOX_HORIZ_PADDING 10


using std::vector;
using std::string;

class sdl_help;

extern sdl_help* sdl_access;

/*! \brief handles the text boxes and the graphical
 *representation of itself. Form's have a vector of these.
 *Only the one being edited is displayed and interactable */
class page{
	public:
		//! default page constructor
		page();

		//! copy constructer here prevents double free errors
		page(const page& other);

		//! frees dynamic memory allocations
		~page();

		//##########################################################################
		//! sets up this page's text boxes and graphics
		void page_init(unsigned int num_columns_in, unsigned int rows_needed,
							const vector<string>& column_labels_in,
							const vector<string>& row_labels_in,
							const vector<int>& column_spacings);

		/*! \brief sets up this page's variables and graphics, with info
		*coming from a config file */
		void init_from_config(unsigned int num_columns_in, unsigned int rows_needed,
								const vector<string>& column_labels_in,
								const vector<string>& row_labels_in,
								const vector<int>& column_spacings,
								const vector<string>& init_values);

		//! helper for page_init, does the mundane pass-through assignments
		void init_local_var(uint num_columns_in,
							uint rows_needed,
							const vector<string>& column_labels_in,
							const vector<string>& row_labels_in);

		//! sets up the row labels, if they exist
		void set_row_labels(const vector<string>& row_labels_in,
								int& x_start_point);

		//! helper for page_init that sets up the text boxes
		void set_text_boxes(int& x_start_point, const vector<int>& column_spacings,
								bool& row_labels_exist);

		//! helper for page_init that sets up the column labels/headers
		void init_column_labels(const vector<int>& column_spacings,
									int& x_start_point,bool& row_labels_exist);
		//##########################################################################

		//! this function draws the pages headers, labels and text boxes
		void draw_me();

		//! this function is a non-const reference getter for the text box vector
		vector<text_box>& get_text_boxes(){
			return text_boxes;
		}

		//! this function is a const reference getter for the text box vector
		const vector<text_box>& get_const_text_boxes(){
			return text_boxes;
		}

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
		//! number of columns that will be needed
		unsigned int num_columns;

		//! number of rows that will be needed
		unsigned int num_rows;

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

		//! store the location of the textures for the row labels
		vector<SDL_Rect> row_label_rects;
};
