//! \file c_tuples.h houses some structs that do little more than hold several values together


#pragma once

//! \struct index_value is a tuple of sorts that stores a text boxes string and index
/*! this is useful for reporting errors found in user inputs */
struct index_value{
	//! this consctructor sets the fields
	index_value(std::string string_in = "", unsigned int index_in = 0){
		value = string_in;
		index = index_in;
	}


	std::string value;     //!< string of the offending text_box
	unsigned int index;    //!< index in the page in which this value was found
};
