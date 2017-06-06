//! \file string+.h \brief implements functions useful on strings, like split(string,delimeter)

#pragma once

#include<string>
#include<vector>
//! this is just a handy function that splits a string along a given character into an array of strings
/*! \param split_me is the string that needs to be smashed into pieces and placed in a vector
 * \param delim is the character along which the function does the smashing. There can be no spaces after
 *the meaningful information, or it will likely return a bad vector */
std::vector<std::string> split(std::string split_me, char delim);

