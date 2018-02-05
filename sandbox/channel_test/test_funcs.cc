


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

void calc_Atarget_Abeam(int IENCH,
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
			cout << "FOP can't handle IENCH = 7 feature." << endl;
			break;

		default:
			cout << "Invalid IENCH value." << endl;
	}
}

void lab_to_ecm(double& ecm_value){

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

//! this function calls the cross-compiled tool for finding Q values
/*! the tool will be in fortran, so the function definition must
 *be done in this way to allow it to be called directly from
 * a c function */
extern "C" {

	void dqv(int* A,int* Z, double return_values[6]);

}


void calc_open_channels(bool open_channels[]){

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
			cout << "Channel: " << c << " is open" << endl;
		}

	}

}

