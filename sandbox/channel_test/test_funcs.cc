#include <iostream>
//#include <fstream>

//for system calls to compile Tom's Q value calculating code
//#include <cstdlib> 

using namespace std;

double find_elab(){

	double elab_val = 0;

	cout << "Please enter the ELAB value." << endl;

	cin >> elab_val;

	return elab_val;

}

int find_icm(){

	int icm_val = -1;

	cout << "Please enter 0 if the previous input was lab energy." << endl;
	cout << "Or enter 1 if the previous input was already center of mass"
		 << " energy." << endl;

	cin >> icm_val;

	return icm_val;
}

void find_compound_A_Z(unsigned int& compound_A,
										unsigned int& compound_Z){

	cout << "Please enter compound A." << endl;

	cin >> compound_A;

	cout << "Please enter compound Z." << endl;

	cin >> compound_Z;

}

int find_IENCH(){

	//store IENCH's value to be returned from this function
	int return_me;

	cout << "Please enter IENCH." << endl;
	cout << "IENCH is the entrance channel, where: " << endl;
	cout << " 1 = neutron\n"
		 << " 2 = proton\n"
		 << " 3 = deuteron\n"
		 << " 4 = triton\n"
		 << " 5 = helium-3\n"
		 << " 6 = alpha" << endl;
	cin >> return_me;

	return return_me;
}

void calc_Ap_Zp_At_Zt(int IENCH,
						const int compound_A,const int compound_Z,
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

/*
bool check_hfqvalue(){

	ifstream tool_check;
	tool_check.open("./q_val/hf_qvalue");

	if(tool_check.good()){
		cout << "HF_QVALUE EXISTS." << endl;
		return true;

	} else {
		cout << "HF_QVALUE DNE." << endl;
		cout << "Trying make..." << endl;
		system("make -C ./q_val/");
		tool_check.open("./q_val/hf_qvalue");
		if(tool_check.good()){
			cout << "Successfully compiled hf_qvalue tool." << endl;
			return true;
		} else {
			cout << "Failure to creat hf_qvalue tool." << endl;
			return false;
		}
	}

	tool_check.close();


}
*/

/* ftran tool spec
      subroutine cqvalue(izp,iap,izt,iat,qn,qp,qd,qt,        
     >                          q3He,qalpha,tflag)
*/
extern "C"{
 
	void cqvalue_(int* Z_proj,int* A_proj,int* Z_targ,int* A_targ,
					double* qn,double* qp, double* qd, double* qt,
					double* q3He, double* qalpha, int* tflag);

}

void lab_to_ecm(double& ecm_value,const unsigned int& A_targ,
				const unsigned int& A_proj){

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


	converted_ecm = converted_ecm * (A_targ / (A_proj + A_targ));	

	//fill the passed energy parameter with the calculated answer
	ecm_value = converted_ecm;

}

void calc_open_channels(bool open_channels[]){

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

	//used to do the calculation, needs to be calculated from
	//IENCH and A and Z of compound nucleus from Andiamo inputs
	int A_proj = 0, Z_proj = 0;	
	int A_targ = 0, Z_targ = 0;

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


	/*
	//make sure hf_qvalue executable exists
	//quit if it doesn't exist and can't be made
	if(!check_hfqvalue()){
		cout << "Q value calculation tool is absent." << endl;
		exit(-1);
	}

	//craft the shell command to run hfqvalue with the info it needs
	string run_command = "./q_val/hf_qvalue ";
	run_command += 


	system("./q_val/hf_qvalue
	*/

	//####### END PART 1 ######################################################


	//####### PART 2 ##########################################################
	//make sure that we have access to the center of mass energy


	// calculated from inputs to Andiamo with the following formula
	// HF parameter :ICM (TC Reference Frame)
	//	= 0 given energies are in lab, convert to ecm
	//  = 1 given energies are in ecm, no need to convert
	//*************************************************************

	//we need this value for FOP
	double ecm_value = 0;

	//first, we'll assume it's given as center of mass energy
	ecm_value = find_elab();

	//this value will tell us if ELAB's value is in
	//lab energy or center of mass energy
	int ref_frame = find_icm();

	//icm_val == 0 means that we were given lab energy
	//so convert the current value in ecm_value
	//to center of mass energy
	if(ref_frame == 0){

		//function converts lab energy to center of mass energy
		lab_to_ecm(ecm_value,A_targ,A_proj);

	}
	//#######END PART 2 #######################################################



	//###### FINALLY ##########################################################
	//use Qvalue and center of mass energy to figure out which channels
	//are open


	//if Q + Ecm for a channel is > 0, we need to run FOP for that channel
	for(unsigned int c = 0; c < 6; c++){

		double total = q_values[c] + ecm_value;

		//if Q + Ecm > 0, store the fact that we need to run FOP
		//for that particle. Elsewise, just leave it false.
		//(values in open_channels default to false in constructor
		if(total > 0){

			//store the fact that this channel needs to run FOP
			open_channels[c] = true;
			cout << "Channel: " << c << " is open" << endl;
		}

	}

}

