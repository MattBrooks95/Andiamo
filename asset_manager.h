//! \file asset_manager.h describes the resource management class

#pragma once

#include<map>
#include<string>
#include<iostream>
#include<fstream>
#include<regex>  //allows checking if file is a png file

//allows traversing a directory and getting its file names.
//./Assets in this case
#include<dirent.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

class sdl_help;

using std::string;

//! \class asset_manager loads desired images into a dictionary
class asset_manager{

  public:
	//! sets sdl_helper reference, so textures can be made
	asset_manager(/*sdl_help* sdl_helper_in = NULL*/);
	//! copies another asset_manager to this one
	/*! shouldn't ever happen */
	asset_manager(const asset_manager& other);
	//! clean up memory by walking the map of texture pointers
	~asset_manager();

	//! arbitrarily set the pointer to the sdl_helper class
	//void set_sdl_help(sdl_help* sdl_helper_in);

	//! this function crawls through the assets folder, loading all png files
	void pull_assets();
	//! recursive helper for pull_assets
	void pull_helper(const string& subroot_path);
	//! this function retrieves the texture for a string argument
	/*! it first checks the map to see if it has already been loaded.
	 *If it has been loaded, it just returns that texture.
	 *elsewise, it loads the texture into the map, and then
	 *returns it.
	 *\param target is the name of the texture to be retrieved
	 *\return returns the sdl texture, or NULL on error */
	SDL_Texture* get_texture(const std::string& target);

	//! this function loads images
	/*! it creates a temporary surface for the requested image,
	 *and then converts it to a texture and puts it in the texture
	 *map
	 *\param load_me name of image to be loaded - must be in Assets
	 *\return returns the loaded sdl texture, or NULL on error */
	SDL_Texture* load_image(const std::string& load_me);
	//! this function prints the map of textures 
	void list_images(std::ostream& outs);

  private:
		//sdl_help* sdl_helper;
		std::map<std::string,SDL_Texture*> textures;
		unsigned int num_textures;
};




