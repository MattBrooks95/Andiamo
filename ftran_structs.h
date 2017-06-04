//! \file ftran_structs.h declares some structs that make keeping track out outputs easy
#include<iostream>

//! param_int4 is a default c++ integer "and more"
/*! it allows storage of a name that serves a contextual purpose, and a value that is compatible with Fortran INT*4 */
struct param_int4{
	//! constructor with default values
	param_int4(std::string name_in = "No name",int val_in = 0);

	//! overloaded << operator for printing
	friend std::ostream& operator <<(std::ostream& outs, const param_int4& rhs);

	//! I don't know if this works
	/*! Something like this will need to be done when/if we have Andiamo
	 * running the feshbach code in place */
	int* get_ptr();

	//! operator for setting value to be equivalent to a right hand param_real8
	void operator =(const param_int4& other);

	//! operator for setting name equal to a string
	void operator =(std::string other);

	//! operator for setting value equal to a double
	void operator =(double other);

	//! boolean equality check of two param_real8s
	bool operator ==(const param_int4& other);

	//! boolean equality check of name field vs a right hand argument string
	bool operator ==(std::string other);

	//! boolean equality check of value field vs a right hand argument double
	bool operator ==(int other);

	//! this is mostly an error message. Fortran doesn't have type conversion
	void operator =(int other);



	std::string name;
	int value;
};

//! param_real8 is a default c++ double "and more"
/*! it allows storage of a name that serves a contextual purpose, and a value that is compatible with Fortran REAL*8 */
struct param_real8{
	//! constructor with default values
	param_real8(std::string name_in = "No name",double val_in = 0.0);

	//! overloaded << operator for printing
	friend std::ostream& operator <<(std::ostream& outs, const param_real8& rhs);

	//! I don't know if this works
	/*! Something like this will need to be done when/if we have Andiamo
	 * running the feshbach code in place */
	double* get_ptr();

	//! operator for setting value to be equivalent to a right hand param_real8
	void operator =(const param_real8& other);

	//! operator for setting name equal to a string
	void operator =(std::string other);

	//! operator for setting value equal to a double
	void operator =(double other);

	//! boolean equality check of two param_real8s
	bool operator ==(const param_real8& other);

	//! boolean equality check of name field vs a right hand argument string
	bool operator ==(std::string other);

	//! boolean equality check of value field vs a right hand argument double
	bool operator ==(double other);

	//! this is mostly an error message. Fortran doesn't have type conversion
	void operator =(int other);



	std::string name;
	double value;
};

struct param_string{
	param_string(std::string name_in = "No name",std::string value_in = NULL,
		     unsigned int size_in = 0);

	unsigned int size; //!< make sure that the c-style string's size matches fortran's
	std::string name; //!< is the contextual name for the variable, like the label line in an HF input file
	std::string value; //!< is the actual string that should be output and cared about
};


/*
struct param_int8{
	param_int8(std::string name_in,long long int val_in){
		name = name_in;
		value = val_in;
	}
	~param_int8(){;}

	std::string name;
	long long int value;
};
struct param_real4{
	param_real4(std::string name_in,float val_in){
		name = name_in;
		value = val_in;
	}
	~param_real4(){;}

	std::string name;
	float value;
};*/






