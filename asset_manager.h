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
		//! most of the initialization work is done by pull_assets
		asset_manager();

		//! copies another asset_manager to this one
		/*! shouldn't ever happen */
		asset_manager(const asset_manager& other);

		//! clean up memory by walking the map of texture pointers
		~asset_manager();

		//! this function crawls through the assets folder, loading all png files
		/*! I've moved away from using this, because it does work on potentially
		 * unneeded files in the Assets folder */
		void pull_assets();

		//! recursive helper for pull_assets
		void pull_helper(const string& subroot_path);

		//! this function retrieves the texture for a string argument
		/*! it first checks the map to see if it has already been loaded.
		 *If it has been loaded, it just returns that texture.
		 *elsewise, it loads the texture into the map, and then returns it.
		 *\param target is the path relative to the Assets folder
		 *\return returns the sdl texture, or NULL on error */
		SDL_Texture* get_texture(const std::string& target);

		//! this function attempts to load a png, but from an absolute path
		/*! you want get_texture() most of the time, because it searches
		 * relative to the assets folder */
		SDL_Texture* get_absolute_path_texture(const std::string& target);

		uint get_number_of_texture_calls(){ return get_texture_calls;}

		//! this function loads images
		/*! it creates a temporary surface for the requested image,
		 *and then converts it to a texture and puts it in the texture
		 *map. You can use this, but you probably want get_texture();
		 *\param load_me name of image to be loaded - must be full path
		 *\return returns the loaded sdl texture, or NULL on error */
		SDL_Texture* load_image(const std::string& load_me);

		//! this function prints the map of textures
		void list_images(std::ostream& outs);

	private:
		uint get_texture_calls;
		string asset_home_path;
		std::map<std::string,SDL_Texture*> textures;
		unsigned int num_textures;
};
