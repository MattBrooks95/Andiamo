/*! \file helpers.h describes utility functions that aren't
 *connected to any sort of class, except for the error logger */

#pragma once

#include<string>
#include<limits>

#include "logger.h"

using std::string;

extern logger* logger_access;;

//! stoi, with exception handling
/*! \param convert_me is the string to be casted to an integer
 *  \return returns the result of the conversion or the minimum integer
 *possible on error */
int str_to_integer(const string& convert_me);

//! stod, with exception handling
/*! \param convert_me is the string to be casted to a double
 *  \return returns the result of the conversion, or the minimum double
 *  possible on error */
double str_to_double(const string& convert_me);

/*! crafts an error message about converting from string to X type, with an
 *explanatory string at the end.*/
string get_str_conversion_error(const string& bad_str,const string& type,
                                   const string& explanation);