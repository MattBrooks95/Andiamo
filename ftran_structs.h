//! \file ftran_structs.h declares some structs that make keeping track out outputs easy
#pragma once
#include<iostream>
#include<vector> 
#include<string>


using std::ostream;
using std::string;
using std::vector;


//! param_int4 is a default c++ integer "and more"
/*! it allows storage of a name that serves a contextual purpose, and a value that is compatible with Fortran INT*4 */
struct param_int4{
	//! constructor with default values
	param_int4(string name_in = "No name",int val_in = 0);

	//! overloaded << operator for printing
	friend ostream& operator <<(ostream& outs, const param_int4& rhs);

	//! returns a string version of this parameter's information
	string get_string();

	//! I don't know if this works
	/*! Something like this will need to be done when/if we have Andiamo
	 * running the Hauser-Feshbach code in place */
	int* get_ptr();

	//! operator for setting value to be equivalent to a right hand param_real8
	void operator =(const param_int4& other);

	//! operator for setting name equal to a string
	void operator =(string other);

	//! operator for setting value equal to a double
	void operator =(double other);

	//! boolean equality check of two param_real8s
	bool operator ==(const param_int4& other);

	//! boolean equality check of name field vs a right hand argument string
	bool operator ==(string other);

	//! boolean equality check of value field vs a right hand argument double
	bool operator ==(int other);

	//! this is mostly an error message. Fortran doesn't have type conversion
	void operator =(int other);



	string name;//!< contextual name for the parameter. This is how the output is associated with its tile
	int value; //!< integer value to be output after some formatting
};

//! param_real8 is a default c++ double "and more"
/*! it allows storage of a name that serves a contextual purpose, and a value that is compatible with Fortran REAL*8 */
struct param_real8{
	//! constructor with default values
	param_real8(string name_in = "No name",double val_in = 0.0);

	//! overloaded << operator for printing
	friend ostream& operator <<(ostream& outs, const param_real8& rhs);

	//! returns a string version of this parameter's information
	string get_string();

	//! I don't know if this works
	/*! Something like this will need to be done when/if we have Andiamo
	 * running the feshbach code in place */
	double* get_ptr();

	//! operator for setting value to be equivalent to a right hand param_real8
	void operator =(const param_real8& other);

	//! operator for setting name equal to a string
	void operator =(string other);

	//! operator for setting value equal to a double
	void operator =(double other);

	//! boolean equality check of two param_real8s
	bool operator ==(const param_real8& other);

	//! boolean equality check of name field vs a right hand argument string
	bool operator ==(string other);

	//! boolean equality check of value field vs a right hand argument double
	bool operator ==(double other);

	//! this is mostly an error message. Fortran doesn't have type conversion
	void operator =(int other);



	string name;//!< contextual name for the parameter. This is how the output is associated with its tile
	double value;//! double value to be output after some formatting
};
//! this is a simple struct to keep track of a string parameter's "variable name" and string "value"
struct param_string{
	//! the constructor sets fields to a parameter, or a default value if () version is called
	/*!\param name_in the name that name will be set to
	 *\param value_in is the value that value will be set to
	 *\param size_in is the value that the size variable will be set to */
	param_string(string name_in = "No name",string value_in = NULL,
		     unsigned int size_in = 0);

	unsigned int size; //!< make sure that the c-style string's size matches fortran's
	string name; //!< is the contextual name for the variable, like the label line in an HF input file
	string value; //!< is the actual string that should be output and cared about
};

//! this struct contains the information about a fortran array given in HF_config
/*! I'm using c++'s <map> library, so input_maker can store a map of these.
  *That way, they can be easily found with their 'key', most likely a string */
struct param_int4_array{
	param_int4_array(string name_in="no name given",unsigned int size = 1,bool satisfied_in=false);

	//! this returns the 'satisfied' boolean variable
	/*! where true means enough inputs have been filled in
	 *and false means that more inputs are needed */
	bool is_satisfied();
	
	//! this function provides a string for the tile to default to
	/*! this function is wierd compared to the other parameters because it is not a simple string
	 *that can have the default value be stored in the field object. It stores an array of parameters,
	 *so special care needs to be taken with how it is initialized to the default value from the config file
	 *in input_maker, but then the field itself gets it in a simple string form. It will then need to be
	 *changed back to a vector for the output functions */
	string get_string();

	//! satisfied should start off false, then become true when size = values.size() (all values filled in)
	bool satisfied;
	unsigned int size;          //!< size of the array as specified by HF_config file - should never change

	string name; //! contextual name for the array, like NENT or LMAX
	vector<int> values;//!< is the array of integers 
};
//! this struct contains the information about a the fortran values in line 4-B in the input manual
/*! these data types will function largely the same as param_int4_array, but will need different formating.
 *I'm not really sure what 8E10.3 means, so I'm going to operate under the assumption that it means
 *8 floating point numbers with a field width of 10 and 3 decimal places. */
struct param_r8_array{
	param_r8_array(string name_in ="no name given", unsigned int size_in = 1, bool satisfied_in=false);
	
	//! this member convers the vector of double values into a single, comma separated list contained in a string
	string get_string();

	bool satisfied; //!< should be true if array has the correct size and legal values, false elsewise
	vector<double> values;//!< store an indefinite amount of numbers with decimal places
	string name; //!< contextual name for the parameter, TIN in this case
	unsigned int size;//! size of the arrayf

};
/*
struct param_int8{
	param_int8(string name_in,long long int val_in){
		name = name_in;
		value = val_in;
	}
	~param_int8(){;}

	string name;
	long long int value;
};
struct param_real4{
	param_real4(string name_in,float val_in){
		name = name_in;
		value = val_in;
	}
	~param_real4(){;}

	string name;
	float value;
};*/






