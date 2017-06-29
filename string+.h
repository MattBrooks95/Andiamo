//! \file string+.h \brief implements functions useful on strings, like split(string,delimeter)

#pragma once

#include<string>
#include<vector>
#include<cstdlib>
//! this is just a handy function that splits a string along a given character into an array of strings
/*! \param split_me is the string that needs to be smashed into pieces and placed in a vector
 * \param delim is the character along which the function does the smashing. There can be no spaces after
 *the meaningful information, or it will likely return a bad vector */
std::vector<std::string> split(std::string split_me, char delim);

//! this function trims the first argument's size by the lenght of the second argument.
/* this function is used to ensure that the character array field::temp_input is the same size as the one expected
 *in input_maker's fortran string struct
 * \param base_string the string who will lose size(string_remove) characters
 * \param remove_num integer value that is the n characters to be "lopped off" the end of the string */
void trim(std::string& base_string,int remove_num);

//! this function splits a comma-separated list of numbers (in a string) and fills a passed vector with the numbers
/*! it removes the quotation marks, then splits along the commas and fills the passed vector with integers */
void handle_i4_array(std::string& string_in,std::vector<int>& fill_me);
