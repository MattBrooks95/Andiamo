//! \file button.cc implements the functions declared in buttons.h

#include "button.h"
using namespace std;

extern asset_manager* asset_access;

//###################### BUTTON class ########################################
button::button(){
    width  = 0;
    height = 0;
    xloc   = 0;
    yloc   = 0;

    my_rect = {0,0,0,0};

    shown = true;

    image_path = "Need to run button::init() on this object";

    button_texture = NULL;
}

void button::init(const string& image_path_in){
    image_path = image_path_in;

    button_texture = asset_access->get_texture(image_path);
    if(button_texture == NULL) logger_access->push_error(string(SDL_GetError()));

    SDL_QueryTexture(button_texture,NULL,NULL,&width,&height);

    set_corner_loc();
    make_rect();
}

//virtual
void button::print_me(){
    logger_access->push_msg("image path:"+image_path);
    logger_access->push_msg("xlocation x ylocation:" + to_string(xloc) + "x" + to_string(yloc));
    logger_access->push_msg("width:" + to_string(width) + " height:" + to_string(height));
    logger_access->push_msg(" texture: " + to_string(size_t(button_texture)));
    logger_access->push_msg("shown?:" + to_string(shown));
}

//virtual
void button::make_rect(){
    my_rect.x = xloc;
    my_rect.y = yloc;
    my_rect.w = width;
    my_rect.h = height;
}

//virtual
void button::draw_me(){
    if(shown){
        SDL_RenderCopy(sdl_access->renderer,button_texture,NULL,&my_rect);
    }
}

//virtual
void button::set_corner_loc(){
    xloc = 0;
    //this puts the tile in a fixed position just above the horizonal scroll bar
    yloc = sdl_access->get_h_bar().get_top() - height;
}

//virtual
void button::handle_resize(int yloc_in){
    yloc = yloc_in;
    make_rect();
}

//virtual
void button::force_corner_loc(int xloc_in, int yloc_in){
    xloc = xloc_in;
    yloc = yloc_in;
    make_rect();
}

//################## virtual click members #############
bool button::handle_click(SDL_Event& mouse_event){
    if( was_clicked(mouse_event) ){
        click_helper(mouse_event);
        return true;
    }
    return false;
}

bool button::was_clicked(SDL_Event& mouse_event){

    int mouse_x = mouse_event.button.x;
    int mouse_y = mouse_event.button.y;

    if( (mouse_x > xloc && mouse_x < xloc + width) &&
        (mouse_y > yloc && mouse_y < yloc + height) ){
        return true;
    }
    return false;
}

void button::click_helper(SDL_Event& mouse_event){
    logger_access->push_msg("default button click_helper");
}
//################# virtual click members ###############

//#############################################################################
