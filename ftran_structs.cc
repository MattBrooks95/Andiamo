//! \file ftran_structs.cc \brief this file implements the functions declared in ftran_structs.h
#include "ftran_structs.h"
using namespace std;

//######################### FTRAN STRING ###############################################
param_string::param_string(string name_in,string value_in,unsigned int size_in){
	name = name_in;
	value = value_in;
	size = size_in;
}

//######################################################################################

//######################### INT*4 VARIABLE #############################################
param_int4::param_int4(string name_in,int val_in){
	name = name_in;
	value = val_in;
}

ostream& operator <<(ostream& outs, const param_int4& rhs){
	outs << rhs.name << " " << rhs.value;
	return outs;
}

int* param_int4::get_ptr(){
	return &value;
}

void param_int4::operator =(const param_int4& other){
	name = other.name;
	value = other.value;
}

void param_int4::operator =(string other){
	name = other;
}

void param_int4::operator =(int other){
	value = other;
}

bool param_int4::operator ==(const param_int4& other){
	if(value == other.value) return true;
	return false;
}

bool param_int4::operator ==(string other){
	if(name == other) return true;
	return false;
}

bool param_int4::operator ==(int other){
	if(value == other) return true;
	return false;
}

void param_int4::operator =(double other){
	cout << "Error! Fortran struct: " << this->name
	     << " can not have a float value assigned to it." << endl; 
}
//########################################################################################


//######################### REAL*8 VARIABLE #############################################
param_real8::param_real8(string name_in,double val_in){
	name = name_in;
	value = val_in;
}

ostream& operator <<(ostream& outs, const param_real8& rhs){
	outs << /*rhs.name << " " <<*/ rhs.value;
	return outs;
}

double* param_real8::get_ptr(){
	return &value;
}

void param_real8::operator =(const param_real8& other){
	name = other.name;
	value = other.value;
}

void param_real8::operator =(string other){
	name = other;
}

void param_real8::operator =(double other){
	value = other;
}

bool param_real8::operator ==(const param_real8& other){
	if(value == other.value) return true;
	return false;
}

bool param_real8::operator ==(string other){
	if(name == other) return true;
	return false;
}

bool param_real8::operator ==(double other){
	if(value == other) return true;
	return false;
}

void param_real8::operator =(int other){
	cout << "Error! Fortran struct: " << this->name
	     << " can not have an integer assigned to it." << endl; 
}
//########################################################################################

//###################### INT4 ARRAY ######################################################
param_int4_array::param_int4_array(string name_in,unsigned int size_in,bool satisfied_in){
	name = name_in;
	size = size_in;
	satisfied = satisfied_in;
}

bool param_int4_array::is_satisfied(){
	return satisfied;
}
string param_int4_array::get_string(){
	string return_me; //declare a string
	for(unsigned int c = 0; c < values.size();c++){
		if(c != values.size()-1){//add a comma to the end of all values but the last
			return_me = return_me + to_string(values[c]) + ",";
		} else {
			return_me = return_me + to_string(values[c]);
		}
	}
	return return_me;
}
//########################################################################################

//################## E? ARRAY ############################################################
param_e_array::param_e_array(string name_in, unsigned int size_in, bool satisfied_in){
	name = name_in;
	satisfied = satisfied_in;
	size = size_in;

}
string param_e_array::get_string(){
	string return_me;
	for(unsigned int c = 0; c < values.size();c++){
		string number_bit =  to_string(values[c]);

		//index for formatting loop
		unsigned int rear_index = 1;
		//for(;rear_index < number_bit.size();rear_index++){
	
			while( number_bit[number_bit.size()-rear_index]  == '0' && 
			    number_bit[number_bit.size()-1-rear_index] != '.'
			   ){
				number_bit.pop_back();//remove extra zeros, but always leave one
							     //after decimal
			}

		//}
		return_me = return_me + number_bit;

		//add a comma to all values in the list but the last one
		if(c != values.size()-1){
			return_me = return_me + ",";
		}

	}
	return return_me;

}
//########################################################################################








