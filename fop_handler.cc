//! \file fop_handler.cc implements the class defined in fop_handler.h

#include "system_wrapper.h"
#include "fop_handler.h"
#include "sdl_help.h"
#include "input_maker.h"
#include "button_manager.h"
#include "logger.h"

using namespace std;

extern system_wrapper* system_access;

fop_handler::fop_handler(){

	string home_path = system_access->get_home();
	omp_path     = home_path + OMP_PATH;

	trans_path   = home_path + TRANSMISSION_PATH;

	scratch_path = home_path + SCRATCH_PATH;

	//start off assuming all channels are inactive
	open_channels[0] = false;
	open_channels[1] = false;
	open_channels[2] = false;
	open_channels[3] = false;
	open_channels[4] = false;
	open_channels[5] = false;
	open_channels[6] = false;

    //no transmission coefficients are ready in between the fop_handler
    //instantiation and fop_main being ran
    ready = false;
}

fop_handler::~fop_handler(){

    for(vector<deck*>::iterator it = fop_decks.begin();
        it != fop_decks.end();
        it++){

        delete(*it);
    }

}

void fop_handler::fop_main(){

	int A_proj = 0, Z_proj = 0;
	int A_targ = 0, Z_targ = 0;
    //fop needs this (potentially converted) beam energy
    double ecm_value = 0;
	//figure out which channels are possible
	calc_open_channels(A_proj,Z_proj,A_targ,Z_targ,ecm_value);

	cout << "A Projectile: " << A_proj << " Z Projectile: " << Z_proj << endl;
	cout << "A Target: " << A_targ << " Z Target: " << Z_targ << endl;


	//prepare the "decks" of "cards" necessary to run FOP per channel
	for(unsigned int c = 0; c < 6; c++){

		if(open_channels[c]){
			prepare_deck(A_proj,Z_proj,A_targ,Z_targ,ecm_value);
		}

	}

    //I think this is wrong and needs cut ##############################
	//set a path to an automated FOP output file,
	//OR allow the user to enter a custom one (not implemented yet)
	//most_recent_FOP_out = make_FOP_pairs(A_proj,Z_proj,A_targ,Z_targ);
    //##################################################################

    make_FOP_pair(A_proj,Z_proj,A_targ,Z_targ);
    cout << "File pair for this calculation is:\n"
         << fop_file_names.first << ":" << fop_file_names.second << endl;

	/*ofstream FOP_input_file;
	FOP_input_file.open(scratch_path + most_recent_FOP_out);
	if(FOP_input_file.fail()){
		cout << "Couldn't make output stream to: "
			 << scratch_path + most_recent_FOP_out << endl;
	}*/

    //for each open channel, come up with the
    //(FOP input file name, fop output file name) pairs
    for(unsigned int c = 0; c < 6; c++){

       if(open_channels[c]){
            ;
        }
    }


	//run FOP per open channel
	for(unsigned int c = 0; c < 6; c++){

		if(open_channels[c]){
			run_fop();
		}
	}


    //set ready to true, transmission coefficients have been prepared
    ready = true;
}




void fop_handler::calc_open_channels(int& A_proj, int& Z_proj,
									 int& A_targ, int& Z_targ,
                                     double& ecm_value){


	//########### PART ONE ##################################################//
	//use hfqvalue to acqure q values for the different exit channels

	//Find A for beam and target
	//from the compound A and Z

	//need to get compound A and Z from Andiamo inputs
	unsigned int compound_A, compound_Z;

	//read in these values from standard input, in Andiamo
	//these will be found in parameter maps
	find_compound_A_Z(compound_A,compound_Z);

	//now we need to grab IENCH, will be read in from console
	//in Andiamo, will come from parameter map
	int IENCH = find_IENCH();

	//use IENCH mode and the given compound information
	//to calculate A & Z of target and A & Z of projectile/beam
	calc_Ap_Zp_At_Zt(IENCH,compound_A,compound_Z,A_proj,Z_proj,A_targ,Z_targ);

	//use Tom's tool, cqvalue.f, to calculate q for each channel

	//container for q values, with
	/*  0 = neutron
		1 = proton
		2 = deuteron
		4 = triton
		5 = helium-3
		6 = alpha
	*/
	double q_values[6];

	int ftran_flag = 0;

	//use Tom's fortran tool to calculate the Q values for each channel
	cqvalue_(&Z_proj,&A_proj,&Z_targ,&A_targ,&q_values[0],
			 &q_values[1],&q_values[2],&q_values[3],&q_values[4],
			 &q_values[5],&ftran_flag);
	cout << "Results from Q value calculation: " << endl;
	for(unsigned int c = 0; c < 6; c++){
		cout << "Value " << c << ": " << q_values[c] << endl;
	}
	//####### END PART 1 ######################################################


	//####### PART 2 ##########################################################
	//make sure that we have access to the center of mass energy


	// calculated from inputs to Andiamo with the following formula
	// HF parameter :ICM (TC Reference Frame)
	//	= 0 given energies are in lab, convert to ecm
	//  = 1 given energies are in ecm, no need to convert
	//*************************************************************

    //ecm_value was passed to and needs filled by this function
	//first, we'll assume it's given as center of mass energy
	ecm_value = find_elab();

	//this value will tell us if ELAB's value is in
	//lab energy or center of mass energy
	int ref_frame = find_icm();

	//icm_val == 0 means that we were given lab energy
	//so convert the current value in ecm_value
	//to center of mass energy
	if(ref_frame == 0){

		cout << "Converting Elab value." << endl;
		cout << "Given value: " << ecm_value << endl; 
		//function converts lab energy to center of mass energy
		lab_to_ecm(ecm_value,A_targ,A_proj);
		cout << "Value after Elab -> Ecm conversion: " << ecm_value << endl;
	}
	//#######END PART 2 #######################################################

	//###### FINALLY ##########################################################
	//use Qvalue and center of mass energy to figure out which channels
	//are open

	//if Q + Ecm for a channel is > 0, we need to run FOP for that channel
	for(unsigned int c = 0; c < 6; c++){

		cout << "Q_value: " << q_values[c] << " Ecm value: "
			 << ecm_value << endl;

		double total = q_values[c] + ecm_value;

		//if Q + Ecm > 0, store the fact that we need to run FOP
		//for that particle. Elsewise, just leave it false.
		//(values in open_channels default to false in constructor)
		if(total > 0){

			//store the fact that this channel needs to run FOP
			open_channels[c] = true;
			cout << "Channel: " << c << " is open. ";
			cout << "Q + Ecm value: ";
			cout.precision(15);
			cout << total << "\n"  << endl;
		} else {

			cout << "Channel: "  << c << " is not open.";
			cout << "Q + Ecm value: ";
			cout.precision(15);
			cout << total << "\n" << endl;
		}
	}


}


void fop_handler::make_FOP_pair(const int& A_proj, const int& Z_proj,
							     const int& A_targ, const int& Z_targ){

	string in_temp_file_name;

	in_temp_file_name = to_string(A_proj)   + "_" + to_string(Z_proj)
					 + "_" + to_string(A_targ) + "_" + to_string(Z_targ)
					 + "_fop_in" + ".txt";

	cout << "Temp FOP input file name: " << in_temp_file_name << endl;
    
	string out_temp_file_name;

	out_temp_file_name = to_string(A_proj)   + "_" + to_string(Z_proj)
					 + "_" + to_string(A_targ) + "_" + to_string(Z_targ)
					 + "_fop_out" + ".txt";

	cout << "Temp FOP output file name: " << out_temp_file_name << endl;
    fop_file_names = pair<string,string>(in_temp_file_name,out_temp_file_name);
}




double fop_handler::find_elab(){

	double elab_val = 0;

	try{

		//get ELAB string value from field, then convert to int
		string elab_str;
		elab_str = tile_access->fields.at("line_2").at("ELAB")->my_text_box.text;
		elab_val = stod(elab_str);

	} catch(out_of_range& map_error){

		output_access->push_error("Couldn't find ELAB in fields map.");

	} catch(invalid_argument& stod_error){

		output_access->push_error("Couldn't convert ELAB string to double.");

	}

	return elab_val;

}

int fop_handler::find_icm(){

	int icm_val = -1;

	try{

		//get icm string value, convert to int
		string icm_str;
		icm_str   = tile_access->fields.at("line_2").at("ICM")->my_text_box.text;
		icm_val = stoi(icm_str);

	} catch(out_of_range& map_error){

		output_access->push_error("Couldn't find ICM in the fields map.");

	} catch(invalid_argument& stoi_error){

		output_access->push_error("Couldn't convert ICM string value to int.");
	}

	return icm_val;

}

void fop_handler::lab_to_ecm(double& ecm_value, const unsigned int& A_targ,
							 const unsigned int& A_proj){


	double converted_ecm = ecm_value;

	converted_ecm = converted_ecm * (A_targ / (double) (A_proj + A_targ));

	//fill the passed energy parameter with the calculated answer
	ecm_value = converted_ecm;

}

void fop_handler::find_compound_A_Z(unsigned int& compound_A,
										unsigned int& compound_Z){

	try{

		compound_A = stoi(tile_access->fields.at("line_2").at("A")->my_text_box.text);
		compound_Z = stoi(tile_access->fields.at("line_2").at("Z")->my_text_box.text);

	} catch(out_of_range& map_error){
		output_access->push_error("Couldn't find A or Z in field map.");
	} catch(invalid_argument& stoi_error){
		output_access->push_error("Couldn't convert A or Z from string to int.");
	}

	cout << "Compound A from user: " << compound_A << endl;
	cout << "Compound Z from user: " << compound_Z << endl;

}

int fop_handler::find_IENCH(){

	//store IENCH's value to be returned from this function
	int return_me;
	string iench_str;

	try{
		iench_str = tile_access->fields.at("line_2").at("IENCH")->my_text_box.text;
		return_me = stoi(iench_str);
	} catch(out_of_range& map_error){
		output_access->push_error("Couldn't find IENCH in field map.");
	} catch(invalid_argument& stoi_error){
		output_access->push_error("Couldn't convert IENCH from string to int.");
	}

	cout << "IENCH value: " << return_me << endl;
	return return_me;
}

void fop_handler::calc_Ap_Zp_At_Zt(int IENCH,
								const int compound_A, const int compound_Z,
								int& A_proj, int& Z_proj,
								int& A_targ, int& Z_targ){


	switch(IENCH){

		case 1: //neutron

			A_proj   = 1;
			Z_proj   = 0;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z;
			break;

		case 2: //proton

			A_proj = 1;
			Z_proj = 1;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z - Z_proj;
			break;

		case 3: //deuteron

			A_proj = 2;
			Z_proj = 1;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z - Z_proj;
			break;

		case 4: //triton

			A_proj = 3;
			Z_proj = 1;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z - Z_proj;
			break;

		case 5: //3He

			A_proj = 3;
			Z_proj = 2;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z - Z_proj;
			break;

		case 6: //alpha

			A_proj = 4;
			Z_proj = 2;

			A_targ = compound_A - A_proj;
			Z_targ = compound_Z - Z_proj;
			break;

		case 7:
			cout << "FOP can't handle IENCH = 7 feature." << endl;
			break;

		default:
			cout << "Invalid IENCH value." << endl;
	}

}

void fop_handler::prepare_deck(int A_proj,int Z_proj,int A_targ,int Z_targ,
                               double ecm_value){
    //start off assuming we'll use the default TC directory
    string transmission_dir = trans_path;


    //if the user has specified an alternate directory, 
    //use that instead. Note that a full path must be provided here.
    //The alternate directory must also have the same file structure
    //as the provided default one.
    string alt_tc_dir = button_access->get_tc_dir_button().my_text_box.text;
    if(alt_tc_dir.size() != 0){
        cout << "using alternate TC dir:" + alt_tc_dir << endl; 
        transmission_dir = alt_tc_dir;
    }

    

	//cout << "Stand in for prepare_decks() work." << endl;

    //push NULL ptr into back of fop_vector
    fop_decks.push_back(NULL);
    fop_decks.back() = new deck();

    //#### initialize it's cards to the proper values ####

    //get a reference to the uninitialized fop deck we just made
    deck* this_deck = fop_decks.back();

    //c needs calculations #####################################
    /* EMax  = (EBeam * (ATarget / (ABeam * ATarget)))
     *         * ((ARecoil + AC)/(AR))
     * EStep = EMax / N, where we will set N to be 20
     * EMin = Emax - (N-1) * EStep  */
    //double EMax = 

    //##########################################################

    //d comes from andiamo input, projectile info
    string D_args(to_string(Z_proj));
    D_args += "," + to_string(Z_proj);
    //this_deck->at("D")->info = 

    //e also comes from andiamo input, target info
    //f can be left alone, defaults to all 0's (no fitting)
    //l can be left alone, just tells FOP to do TCs (0,3)

    //s,t,u,v need initialized using data from giant directory
}

void fop_handler::run_fop(){

	cout << "Stand in for run_fop() work." << endl;
    cout << "FOP  input file: " << fop_file_names.first << endl;
    cout << "FOP output file: " << fop_file_names.second << endl;
}













