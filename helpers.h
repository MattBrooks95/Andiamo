/*! \file helpers.h describes utility functions that aren't
 *connected to any sort of class, except for the error logger */

#pragma once

#include<string>

#include "logger.h"

using std::string;

extern logger error_logger;

//! stoi, with exception handling
int str_to_integer(const string& convert_me);

//! stod, with exception handling
double str_to_double(const string& convert_me);

/*! crafts an error message about converting from string to X type, with an
 *explanatory string at the end.*/
string get_str_conversion_error(const string& bad_str,const string& type,
                                   const string& explanation);