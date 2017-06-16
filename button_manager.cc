//! \file this file implements the class described in button_manager.h

#include "button_manager.h"
using namespace std;


button_manager::button_manager(sdl_help* sdl_helper_in){
	sdl_helper = sdl_helper_in;
	button_image_p = "./Assets/Images/Buttons/";

	tray_image_name = "button_tray.png";

	tray_shown = true;

	tray_rect.x = 0;
	tray_rect.y = 0;
	tray_rect.w = 0;
	tray_rect.h = 0;

	button_tray_surf = NULL;
	button_tray_texture = NULL;
}

button_manager::~button_manager(){
	SDL_FreeSurface(button_tray_surf);
	SDL_DestroyTexture(button_tray_texture);
}

void button_manager::init_tray(){
	//cout << (button_image_p+tray_image_name).c_str() << endl; 
	button_tray_surf = IMG_Load( (button_image_p+tray_image_name).c_str() );
	if(button_tray_surf == NULL) cout << SDL_GetError() << endl;
	button_tray_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,button_tray_surf);
	if(button_tray_texture == NULL) cout << SDL_GetError() << endl;

	//use query texture to get the texture's height and width
	SDL_QueryTexture(button_tray_texture,NULL,NULL,&tray_rect.w,&tray_rect.h);


	tray_rect.x = 5;//give it a little bit of distance from the right edge of the screen
	tray_rect.y = sdl_helper->get_h_bar().get_top() - (tray_rect.h + 10);//set it to be just above the bottom scroll bar

}


void button_manager::init_buttons(){
	//initialize the placeholder buttons
	default_test.init("default_button.png",button_image_p,sdl_helper);
	output_fname.init("output_name_button.png",button_image_p,sdl_helper);
	t_coefficients.init("tc_file_button.png",button_image_p,sdl_helper);
	graphing_options.init("graphing_options.png",button_image_p,sdl_helper);
	lets_go.init("lets_go.png",button_image_p,sdl_helper);

	//exit dialogue is a special snowflake, handles its own location
	exit_dialogue.init("exit_button.png",button_image_p,sdl_helper);

	int end_of_last_button = 0;//keep track of where the last button ended

	//assign their locations
	default_test.force_corner_loc( tray_rect.x+5,tray_rect.y+7 );

		//keep track of where the next button placement should start
		end_of_last_button = end_of_last_button + tray_rect.x+5 + default_test.get_width();
	
	//these two are thin enough to occupy the same horizontal space, with one above and one below
	output_fname.force_corner_loc( end_of_last_button+5, tray_rect.y + 7);
	t_coefficients.force_corner_loc( end_of_last_button+5, tray_rect.y+7 + output_fname.get_height()+10);

		end_of_last_button = end_of_last_button+5+output_fname.get_width();

	graphing_options.force_corner_loc(end_of_last_button+5,tray_rect.y + 7);

		end_of_last_button = end_of_last_button+5+graphing_options.get_width();

	lets_go.force_corner_loc( end_of_last_button+5, tray_rect.y + 7 );


}
void button_manager::print_buttons(){
	cout << "####################### PRINTING BUTTONS ############################" << endl;
	default_test.print_me();
	cout << endl;
	exit_dialogue.print_me();
	cout << endl;
	output_fname.print_me();
	cout << endl;
	t_coefficients.print_me();
	cout << endl;
	lets_go.print_me();
	cout << endl;
	graphing_options.print_me();
	cout << endl;
	cout << "####################### DONE PRINTING BUTTONS #######################" << endl;



	button default_test;//!< example of a base class instantiation, not actually for use
	button output_fname;//!< button to allow user to select where to output the HF input file
	button t_coefficients;//!< button to allow user to select the input transmission coefficients file
	button lets_go;//!< button to generate output
	button graphing_options;//!< button to allow user to select graphing options
	exit_button exit_dialogue; //! first class that inherits from button default class, handles exiting
}

void button_manager::draw_tray(){
	if(tray_shown){
		SDL_RenderCopy(sdl_helper->renderer,button_tray_texture,NULL,&tray_rect);
	}
}

void button_manager::draw_buttons(){
	draw_tray();
	default_test.draw_me();
	output_fname.draw_me();	
	t_coefficients.draw_me();
	graphing_options.draw_me();
	lets_go.draw_me();

}

void button_manager::click_handling(SDL_Event& mouse_event){
	if(default_test.shown){
		default_test.handle_click(mouse_event);
	}
	if(output_fname.shown){
		output_fname.handle_click(mouse_event);
	}
	if(t_coefficients.shown){
		t_coefficients.handle_click(mouse_event);
	}
	if(lets_go.shown){
		lets_go.handle_click(mouse_event);
	}
	if(graphing_options.shown){
		graphing_options.handle_click(mouse_event);
	}


}









