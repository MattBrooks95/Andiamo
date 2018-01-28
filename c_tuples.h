/*! \file c_tuples.h houses some structs that do little more than hold
 *several values together */


#pragma once

using std::string;

//! \struct index_value is a tuple that stores a text box's string and index
/*! this is useful for reporting errors found in user inputs */
struct index_value{
	//! this consctructor sets the fields
	index_value(string string_in = "", unsigned int index_in = 0){
		value = string_in;
		index = index_in;
	}

	//!< string of the offending text_box
	string value;
	//!< index in the page in which this value was found
	unsigned int index;
};
