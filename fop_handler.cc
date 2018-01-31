//! \file fop_handler.cc implements the class defined in fop_handler.h

#include "fop_handler.h"
#include "sdl_help.h"
#include "input_maker.h"
#include "logger.h"
using namespace std;

fop_handler::fop_handler(){	

	//start off assuming all channels are inactive
	open_channels[0] = false;
	open_channels[1] = false;
	open_channels[2] = false;
	open_channels[3] = false;
	open_channels[4] = false;
	open_channels[5] = false;
	open_channels[6] = false;
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

			//readdir is kind of like a getline statement,
			//read in info then act on it
			while( (file_in_dir = readdir(dir_point)) ){

				//this ensures that only regular files are considered,
				//and not the . and .. directories
				//that exist in nearly every linux directory (but hidden)
				if( file_in_dir->d_type == DT_REG){

					if(c == 0){

						optical_model_files.push_back(file_in_dir->d_name);

					} else if(c == 1){ 

						tc_files.push_back(file_in_dir->d_name);

					} else if(c == 2){

						scratch_files.push_back(file_in_dir->d_name);
					}
				}
			}

			//close the directory
			closedir(dir_point);

		} else {
			error_logger.push_error("Failure to open the /error_logs file, for cleaning by cleaning_check()");
		}
	}

}

void fop_handler::calc_open_channels(){
	//subroutine desep(iz1,ia1,iz2,ia2, nr,jflag,kflag,value,error)
	//           desep(int,int,int,int,int,  int,  int,real8,real8)

	//array structure of q values lines up with booleans for channels
	//in open_channels 
	//double q_values[7];
	//double* value = new double;

	//Tom will give me a new tool, dqv, to give back an array of doubles
	//for the q values for neutron proton deuteron triton 3He alpha

	//passed to dqv, gets filled in with q_values
	double q_values[6];

	/* calculated from inputs to Andiamo with the following formula
	 *
	 *	Ecm =            Atarget		
	 *			Elab *  ---------
	 *                  Abeam + Atarget 
     *
	 * Where: Ecm  = "Center of Mass Energy"
     *        Elab = "Lab Energy" (an Andiamo input from user)
	 *        Atarget = "Mass of Target"
	 *        Abeam   = "Mass of Beam/Projectile"
	 *********************************************************** */
	double ecm_values[6];

	//if Q + Ecm for a channel is > 0, we need to run FOP for that channel

	for(unsigned int c = 0; c < 6; c++){

		double total = q_values[c] + ecm_values[c];

		//if Q + Ecm > 0, store the fact that we need to run FOP
		//for that particle. Elsewise, just leave it false.
		//(values in open_channels default to false in constructor
		if(total > 0){

			//store the fact that this channel needs to run FOP
			open_channels[c] = true;
		}

	}

}

void fop_handler::prepare_decks(){



}

void fop_handler::run_fop(){



}

void fop_handler::print_file_list(){
	cout << "#### FOLDER: " << OMP_PATH << " ####" << endl;
	for(unsigned int c = 0; c < optical_model_files.size(); c++){
		cout << optical_model_files[c] << endl;
	}
	cout << "######################################" << endl;

	cout << "#### FOLDER: " << TRANSMISSION_PATH
		 << " ####" << endl;
	for(unsigned int c = 0; c < tc_files.size(); c++){
		cout << tc_files[c] << endl;
	}
	cout << "#################################################" << endl;

	cout << "#### FOLDER: " << SCRATCH_PATH << " ####" << endl;

	for(unsigned int c = 0; c < scratch_files.size(); c++){
		cout << scratch_files[c] << endl;
	}
	cout << "###################################" << endl;
}
//#############################################################################













