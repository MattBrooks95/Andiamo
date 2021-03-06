#include "ftran_structs.h"
using namespace std;

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
	outs << rhs.name << " " << rhs.value;
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





