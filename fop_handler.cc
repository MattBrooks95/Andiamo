//! \file fop_handler.cc implements the class defined in fop_handler.h

#include "fop_handler.h"
#include "sdl_help.h"
#include "input_maker.h"
#include "logger.h"
using namespace std;

fop_handler::fop_handler(){	
}

fop_handler::~fop_handler(){


}

void fop_handler::get_files_list(){

	string omp_path     = OMP_PATH;
	string trans_path   = TRANSMISSION_PATH;
	string scratch_path = SCRATCH_PATH;

	vector<string> directories;
	directories.push_back(omp_path);
	directories.push_back(trans_path);
	directories.push_back(scratch_path);

	//fill the vectors for this class by reading the directories
	for(unsigned int c = 0; c < 3; c++){
		
		//this allows the opening of a directory as if it were a file
		DIR* dir_point; 
		struct dirent *file_in_dir;
		dir_point = opendir(directories[c].c_str());
		if(dir_point != NULL){

			//readdir is kind of like a getline statement, read in info then act on it
			while( (file_in_dir = readdir(dir_point)) ){

				//this ensures that only regular files are considered, and not the . and .. directories
				//that exist in nearly every linux directory (but hidden)
				if( file_in_dir->d_type == DT_REG){
					if     (c == 0) optical_model_files.push_back(file_in_dir->d_name);
					else if(c == 1) transmission_coefficients_files.push_back(file_in_dir->d_name);
					else if(c == 2) scratch_files.push_back(file_in_dir->d_name);
				}
			}

			closedir(dir_point);//close the directory

		} else {
			error_logger.push_error("Failure to open the /error_logs file, for cleaning by cleaning_check()");
		}
	}

}

void fop_handler::run_fop(){



}

void fop_handler::print_file_lists(){
	cout << "############## FOLDER: " << OMP_PATH << " ###############" << endl;
	for(unsigned int c = 0; c < optical_model_files.size(); c++){
		cout << optical_model_files[c] << endl;
	}
	cout << "########################################################" << endl;

	cout << "############## FOLDER: " << TRANSMISSION_PATH
		 << " ##################" << endl;
	for(unsigned int c = 0; c < transmission_coefficients_files.size(); c++){
		cout << transmission_coefficients_files[c] << endl;
	}
	cout << "########################################################" << endl;

	cout << "############## FOLDER: " << SCRATCH_PATH
		 << " ##################" << endl;
	for(unsigned int c = 0; c < scratch_files.size(); c++){
		cout << scratch_files[c] << endl;
	}
	cout << "########################################################" << endl;
}





