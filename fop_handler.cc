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
			string error_msg  = "Failure to open the /error_logs file,";
			error_msg        += "for cleaning by cleaning_check()";
			error_logger.push_error(error_msg);
		}
	}

}

void fop_handler::calc_open_channels(){

	//Tom will give me a new tool, dqv, to give back an array of doubles
	//for the q values for neutron proton deuteron triton 3He alpha

	//passed to dqv, gets filled in with q_values
	double q_values[6];

	/* calculated from inputs to Andiamo with the following formula
	 * HF parameter :ICM (TC Reference Frame)
	 *	= 0 given energies are in lab, convert to ecm
	 *  = 1 given energies are in ecm, no need to convert
	 *********************************************************** */

	//we need this value for FOP
	double ecm_value = 0;

	//first, we'll assume it's given as center of mass energy
	ecm_value = find_elab();

	//figure out if ELAB's value is in lab energy or center of mass energy
	int ref_frame = find_icm();

	//icm_val == 0 means that we were given lab energy
	//so convert the current value in ecm_value
	//to center of mass energy
	if(ref_frame == 0){

		//function converts lab energy to center of mass energy
		lab_to_ecm(ecm_value);

	}

	//if Q + Ecm for a channel is > 0, we need to run FOP for that channel
	for(unsigned int c = 0; c < 6; c++){

		double total = q_values[c] + ecm_value;

		//if Q + Ecm > 0, store the fact that we need to run FOP
		//for that particle. Elsewise, just leave it false.
		//(values in open_channels default to false in constructor
		if(total > 0){

			//store the fact that this channel needs to run FOP
			open_channels[c] = true;
		}

	}

}

double fop_handler::find_elab(){

	double elab_val = 0;

	try{

		//get ELAB string value from field, then convert to int
		string elab_str;
		elab_str = tile_access->fields.at("line_2").at("ELAB").temp_input;
		elab_val = stod(elab_str);

	} catch(out_of_range& map_error){

		error_logger.push_error("Couldn't find ELAB in fields map.");

	} catch(invalid_argument& stoi_error){

		error_logger.push_error("Couldn't convert ELAB string to double.");

	}

	return elab_val;

}

int fop_handler::find_icm(){

	int icm_val = -1;

	try{

		//get icm string value, convert to int
		string icm_str;
		icm_str   = tile_access->fields.at("line_2").at("ICM").temp_input;
		icm_val = stoi(icm_str);

	} catch(out_of_range& map_error){

		error_logger.push_error("Couldn't find ICM in the fields map.");

	} catch(invalid_argument& stoi_error){

		error_logger.push_error("Couldn't convert ICM string value to int.");
	}

	return icm_val;

}

void fop_handler::lab_to_ecm(double& ecm_value){

	//note that when this function is called,
	//ecm value contains the value provided from the user (ELAB),
	//which is the lab energy. We need to convert to
	//center of mass energy

	/*********************************************************
	*	Ecm =            Atarget		
	*			Elab *  ---------
	*                  Abeam + Atarget 
    *
	* Where: Ecm  = "Center of Mass Energy"
    *        Elab = "Lab Energy" (an Andiamo input from user)
	*     Atarget = "Mass of Target"
	*     Abeam   = "Mass of Beam/Projectile"
	*********************************************************/

	//temporarily store ELABS value
	double converted_ecm = ecm_value;

	unsigned int compound_A = 0, compound_Z = 0;

	//get compound values from Andiamo inputs
	find_compound_A_Z(compound_A,compound_Z);

	//now we need to grab IENCH from Andiamo input
	int IENCH = find_IENCH();

	//used to do the calculation, needs to be calculated from
	//IENCH and A and Z of compound nucleus from Andiamo inputs
	int A_target = 0, A_beam = 0;

	//use IENCH mode and the given compound information
	//to calculate A of target and A of projectile/beam
	calc_Atarget_Abeam(IENCH,compound_A,compound_Z,A_target,A_beam);

	converted_ecm = converted_ecm * (A_target / (A_beam + A_target));	

	//fill the passed energy parameter with the calculated answer
	ecm_value = converted_ecm;

}

void fop_handler::find_compound_A_Z(unsigned int& compound_A,
										unsigned int& compound_Z){

	try{

		compound_A = stoi(tile_access->fields.at("line_2").at("A").temp_input);
		compound_Z = stoi(tile_access->fields.at("line_2").at("Z").temp_input);

	} catch(out_of_range& map_error){
		error_logger.push_error("Couldn't find A or Z in field map.");
	} catch(invalid_argument& stoi_error){
		error_logger.push_error("Couldn't convert A or Z from string to int.");
	}


}

int fop_handler::find_IENCH(){

	//store IENCH's value to be returned from this function
	int return_me;
	string iench_str = tile_access->fields.at("line_1").at("IENCH").temp_input;
	try{
		return_me = stoi(iench_str);
	} catch(out_of_range& map_error){
		error_logger.push_error("Couldn't find IENCH in field map.");
	} catch(invalid_argument& stoi_error){
		error_logger.push_error("Couldn't convert IENCH from string to int.");
	}

	return return_me;
}

void fop_handler::calc_Atarget_Abeam(int IENCH,
										int compound_A,int compound_Z,
										int& A_target, int& A_beam){


	switch(IENCH){

		case 1: //neutron

			A_beam   = 1;
			A_target = compound_A - A_beam;
			break;

		case 2: //proton

			A_beam   = 1;
			A_target = compound_A - A_beam;
			break;

		case 3: //deuteron

			A_beam   = 2;
			A_target = compound_A - A_beam;
			break;

		case 4: //triton

			A_beam   = 3;
			A_target = compound_A - A_beam;
			break;

		case 5: //3He

			A_beam   = 3;
			A_target = compound_A - A_beam;
			break;

		case 6: //alpha

			A_beam   = 4;
			A_target = compound_A - A_beam;
			break;

		case 7:
			error_logger.push_error("FOP can't handle IENCH = 7 feature.");
			break;

		default:
			error_logger.push_error("Invalid IENCH value.");
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













