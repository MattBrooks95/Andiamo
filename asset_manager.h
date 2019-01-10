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

		//! crawls through the assets folder, loading all png files
		/*! I've moved away from using this, because it does work on potentially
		 * unneeded files in the Assets folder */
		void pull_assets();

		//! recursive helper for pull_assets
		void pull_helper(const string& subroot_path);

		//! retrieves the texture for a string argument
		/*! it first checks the map to see if it has already been loaded.
		 *If it has been loaded, it just returns that texture.
		 *elsewise, it loads the texture into the map, and then returns it.
		 *\param target is the path relative to the Assets folder
		 *\return returns the sdl texture, or NULL on error */
		SDL_Texture* get_texture(const std::string& target);

		//! retrieves the surface that was used to make the texture
		/*! you need surfaces for dynamically drawing with
		 * SDL_BlitSurface. Instead of getting a pointer to the form in the map,
		 * this function returns a copy of it, because multiple objects
		 * can't be blitting to the same surface pointer or they'd overlap and
		 * they'd all have eachother's drawings */
		SDL_Surface* get_surface(const std::string& target);

		uint get_number_of_texture_calls(){return get_texture_calls;}
		uint get_number_of_surface_calls(){return get_surface_calls;}

		//! prints the map of textures
		void list_images(std::ostream& outs);

	private:

		SDL_Surface* load_surface(const std::string& load_me);

		//! used by get_texture and get_surface
		SDL_Texture* load_image_return_texture(const std::string& load_me);

		//! used by get_texture and get_surface
		SDL_Surface* load_image_return_surface(const std::string& load_me);

		//! used by the load_image_return functions
		SDL_Texture* make_texture_from_surface(SDL_Surface* surface, const string& map_key);

		uint get_texture_calls;
		uint get_surface_calls;
		string asset_home_path;
		std::map<std::string,SDL_Surface*> surfaces;
		std::map<std::string,SDL_Texture*> textures;
		unsigned int num_textures;
};
