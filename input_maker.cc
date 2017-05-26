//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include "input_maker.h"
using namespace std;

input_maker::input_maker(string file_name_in){
	config_p = "./config/";
	file_name = file_name_in;
}

void input_maker::init(){
	ifstream ins;
	ins.open( (config_p+file_name).c_str() );
	if(ins.fail()){
		cout << "Error in input_maker::init(), couldn't find the input file!" << endl;
		return;
	}

	//do some stuff


	ins.close();
}

void input_maker::output(std::ostream& outs){




}











