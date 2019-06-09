//! \file asset_manager.cc implements the asset_manager class

#include "asset_manager.h"
#include "sdl_help.h"
#include "logger.h"
#include "system_wrapper.h"

using namespace std;


extern system_wrapper* system_access;

extern logger* logger_access;;
extern sdl_help* sdl_access;

#define ASSET_FOLDER_NAME "Assets/"

asset_manager::asset_manager(){
	asset_home_path = system_access->get_home() + "/Andiamo/"+ ASSET_FOLDER_NAME;

	num_textures      = 0;
	get_texture_calls = 0;
}

asset_manager::asset_manager(const asset_manager& other){
	num_textures = other.num_textures;
}

asset_manager::~asset_manager(){

	for(map<string,SDL_Texture*>::iterator it = textures.begin();
		it != textures.end();
		++it){

		if(it->second != NULL){
			SDL_DestroyTexture(it->second);
		}
	}

}

SDL_Texture* asset_manager::get_texture(const string& target){
	get_texture_calls++;

	SDL_Texture* temp_texture = NULL;

	string target_path = asset_home_path + target;

	//try to get texture from map
	try {
		temp_texture = textures.at(target_path);
		return temp_texture;
	} catch(out_of_range& not_found){
		//if it wasn't found in the map, load it instead
		return load_image_return_texture(target_path);
	}
}

SDL_Surface* asset_manager::get_surface(const string& target){

	get_surface_calls++;

	SDL_Surface* temp_surface = NULL;

	string target_path = asset_home_path + target;

	return load_surface(target_path);
}

SDL_Texture* asset_manager::load_image_return_texture(const string& load_path){

	SDL_Surface* temp_surface = load_surface(load_path);

	SDL_Texture* temp_texture = make_texture_from_surface(temp_surface,load_path);

	return temp_texture;

}

SDL_Surface* asset_manager::load_image_return_surface(const string& load_path){

	SDL_Surface* temp_surface = load_surface(load_path);

	SDL_Texture* temp_texture = make_texture_from_surface(temp_surface,load_path);

	return temp_surface;

}

SDL_Surface* asset_manager::load_surface(const string& load_path){
	SDL_Surface* temp_surface = IMG_Load(load_path.c_str());
	return temp_surface;
}

SDL_Texture* asset_manager::make_texture_from_surface(SDL_Surface* surface,
														const string& map_key){

	if(surface == NULL){
		string error = "NULL surface given to make_texture_from_surface";
		logger_access->push_error(error);
		return NULL;
	}

	SDL_Texture* temp_texture;
	temp_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,surface);
	textures.insert(pair<string,SDL_Texture*>(map_key,temp_texture));
	return temp_texture;

}

void asset_manager::list_images(ostream& outs){
	outs << "Printing map of textures. Size: " << textures.size() << endl;
	for(map<string,SDL_Texture*>::iterator it = textures.begin();
		it != textures.end();
		++it){
		outs << it->first << " " << it->second << endl;
	}
}
