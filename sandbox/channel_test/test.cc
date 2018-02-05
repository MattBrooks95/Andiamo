


#include <iostream>
#include <string>
#include <cstdlib>

#include "test_funcs.cc"

using namespace std;


void menu();


int main(){

	bool done = false;

	char control;

	while(!done){

		bool open_channels[] = {false,false,false,false,false,false};

		menu();	

		cin >> control;

		switch(control){

			case 'q':
			case 'Q':
				done = true;
				break;
				
			default:
				calc_open_channels(open_channels);				

		}

		cin.clear();


	}
	return EXIT_SUCCESS;

}

void menu(){
	cout << "This program tests the logic that figures out how many times"
		 << " FOP should be run to get transmission coefficients." << endl;

	cout << "Type Q or q to quit this program. "
		 << "Hit any other letter to run a test." << endl;
}





