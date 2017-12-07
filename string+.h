//! \file string+.h \brief implements functions useful on strings, like split(string,delimeter)

#pragma once

#include<string>
#include<vector>
#include<cstdlib>

using std::string;
using std::vector;


//! this is just a handy function that splits a string along a given character into an array of strings
/*! \param split_me is the string that needs to be smashed into pieces and placed in a vector
 * \param delim is the character along which the function does the smashing. There can be no spaces after
 *the meaningful information, or it will likely return a bad vector */
vector<string> split(string split_me, char delim);

//! this function trims the first argument's size by the length of the second argument.
/* this function is used to ensure that the character array field::temp_input is the same size as the one expected
 *in input_maker's fortran string struct
 * \param base_string the string who will lose size(string_remove) characters
 * \param remove_num integer value that is the n characters to be "lopped off" the end of the string */
void trim(string& base_string,unsigned int remove_num);

//! this function splits a comma-separated list of numbers (in a string) and fills a passed vector with the numbers
/*! it removes the quotation marks, then splits along the commas and fills the passed vector with integers */
void handle_i4_array(string& string_in,vector<int>& fill_me);

//! this function splits a comma-separate dlist of numbers (in a string) and fills a passed vector with the numbers
/*! it removes the quotation marks, then splits along commas and fills the passed vector with doubles */
void handle_r8_array(string& string_in,vector<double>& fill_me);

//! this function loops over the passed in string, and builds a new string that excludes the passed character
/*! \param string_in will be looped over, and the good characters stored in an accumulator string that is assigned to string_in at the end of the functoin
 *\param strip_me is the character that will not be in the "new" string */
void strip_char(string& string_in,char strip_me);





