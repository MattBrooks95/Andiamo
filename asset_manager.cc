//! \file asset_manager.cc implements the asset_manager class

#include "asset_manager.h"
#include "sdl_help.h"
#include "logger.h"
#include "system_wrapper.h"

using namespace std;


extern system_wrapper* system_access;

extern logger* output_access;;
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

void asset_manager::pull_assets(){
	struct dirent* file_in_dir;

	DIR* assets_home;
	assets_home = opendir(asset_home_path.c_str());

	if(assets_home != NULL){

		//readdir is kind of like a getline statement, read in info then act on it
		while( (file_in_dir = readdir(assets_home)) ){

			//regular files should not appear in top level of Assets tree
			if( file_in_dir->d_type == DT_REG){
				output_access->push_error("Not loading file in Assets folder.",
										" Files should be in subdirectories.");
			//if it is another dir, call the helper on it
			} else if(file_in_dir->d_type == DT_DIR){

				//don't care about . and .. dirs
				string ignore1 = ".";
				string ignore2 = "..";
				if(ignore1.compare(file_in_dir->d_name) != 0 &&
				   ignore2.compare(file_in_dir->d_name) != 0){

					//cout << "in dir file mode: " << file_in_dir->d_name << endl;
					//build path to new subdirectories for the helper calls
					string path_to_subdir = asset_home_path;
					path_to_subdir += "/";
					path_to_subdir += file_in_dir->d_name;
					pull_helper(path_to_subdir);
				}
			}
		}

	} else {
		output_access->push_error("Couldn't open Assets folder from",
								" asset_manager::pull_assets().");
	}

	closedir(assets_home);
}

void asset_manager::pull_helper(const string& subroot_path){

	DIR* current_root;
	current_root = opendir(subroot_path.c_str());
	dirent* current_file = NULL;
	if(current_root != NULL){

		while( (current_file = readdir(current_root)) ){

			//if it is a regular file, load it if it is a png file
			if( current_file->d_type == DT_REG){
				//cout << "in reg file mode: " << current_file->d_name << endl;

				string texture_path = subroot_path;
				texture_path += "/";
				texture_path += current_file->d_name;
				regex is_png(".*?\\.png");
				if(regex_match(texture_path,is_png)){
					//cout << "Could open file: " << texture_path << endl;
					get_texture(texture_path);
				} else {
					//cout << texture_path << " ain't a PNG file." << endl;
				}

			//if it is another dir, call the helper on it
			} else if(current_file->d_type == DT_DIR){

				//don't care about . and .. dirs
				string ignore1 = ".";
				string ignore2 = "..";
				if(ignore1.compare(current_file->d_name) != 0 &&
				   ignore2.compare(current_file->d_name) != 0){

					//cout << "in dir file mode: " << current_file->d_name << endl;
					//build path to new subdirectories for the helper calls
					string path_to_subdir = subroot_path;
					path_to_subdir += "/";
					path_to_subdir += current_file->d_name;
					pull_helper(path_to_subdir);
				}
			}
		}

	} else {
		output_access->push_error("From pull_helper, could not open: ",
								subroot_path);
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

	try {
		temp_surface = surfaces.at(target_path);
		//dereference the pointer so that the calling code gets a copy
		SDL_Surface* pointer_to_a_copy = new SDL_Surface(*temp_surface);
		return pointer_to_a_copy;

	} catch(out_of_range& not_found){
		return load_image_return_surface(target);
	}

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
	return IMG_Load(load_path.c_str());
}

SDL_Texture* asset_manager::make_texture_from_surface(SDL_Surface* surface,
														const string& map_key){

	if(surface == NULL){
		string error = "NULL surface given to make_texture_from_surface";
		output_access->push_error(error);
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
