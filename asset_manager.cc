//! \file asset_manager.cc implements the asset_manager class

#include "asset_manager.h"
#include "sdl_help.h"
#include "logger.h"

using namespace std;

extern logger error_logger;
extern sdl_help* sdl_access;

asset_manager::asset_manager(/*sdl_help* sdl_helper_in*/){

	//sdl_helper = sdl_helper_in;
	num_textures = 0;

}

asset_manager::asset_manager(const asset_manager& other){
	num_textures = other.num_textures;
}

asset_manager::~asset_manager(){

	for(map<string,SDL_Texture*>::iterator it = textures.begin();
		it != textures.end();
		it++){

		if(it->second != NULL){
			SDL_DestroyTexture(it->second);
		}
	}

}

/*void asset_manager::set_sdl_help(sdl_help* sdl_helper_in){
	//sdl_helper = sdl_helper_in;

}*/

void asset_manager::pull_assets(){
	
	DIR* assets_home;
	assets_home = opendir("./Assets");
	struct dirent* file_in_dir;
	if(assets_home != NULL){

		//readdir is kind of like a getline statement, read in info then act on it
		while( (file_in_dir = readdir(assets_home)) ){

			//regular files should not appear in top level of Assets tree
			if( file_in_dir->d_type == DT_REG){
				error_logger.push_error("Not loading file in Assets folder.",
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
					string path_to_subdir = "./Assets";
					path_to_subdir += "/";
					path_to_subdir += file_in_dir->d_name;
					pull_helper(path_to_subdir);
				}
			}
		}

	} else {
		error_logger.push_error("Couldn't open Assets folder from",
								" asset_manager::pull_assets().");
	}

	closedir(assets_home);
}

void asset_manager::pull_helper(const string& subroot_path){

	//cout << "From pull_helper, path to subroot: " << subroot_path << endl;
	
	DIR* current_root;
	current_root = opendir(subroot_path.c_str());
	dirent* current_file;
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
		error_logger.push_error("From pull_helper, could not open: ",
								subroot_path);
	}

}

SDL_Texture* asset_manager::get_texture(const string& target){

	SDL_Texture* temp_texture = NULL;

	//try to get texture from map
	try{

		temp_texture = textures.at(target);
		return temp_texture;
	} catch (out_of_range& not_found){
		//if it wasn't found in the map, load it instead		
		return load_image(target);
	}

}

SDL_Texture* asset_manager::load_image(const string& load_me){

	SDL_Surface* temp_surface;
	temp_surface = IMG_Load(load_me.c_str());

	SDL_Texture* temp_texture;
	//temp_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,
	//											 temp_surface);
	temp_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,
												 temp_surface);

	if(temp_surface != NULL){
		SDL_FreeSurface(temp_surface);

		textures.insert(pair<string,SDL_Texture*>(load_me,temp_texture));
														
	} else {
		error_logger.push_error("File "+ load_me + " not found in",
								"load_image!.");
	}

	return temp_texture;

}

void asset_manager::list_images(ostream& outs){
	outs << "Printing map of textures. Size: " << textures.size() << endl;
	for(map<string,SDL_Texture*>::iterator it = textures.begin();
		it != textures.end();
		it++){
		outs << it->first << " " << it->second << endl;
	}
}






