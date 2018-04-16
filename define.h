//! \file define.h declares some macros that replace long C++ object types
#pragma once

//! easier type declaration for an unsigned integer
#define uint unsigned int

//! shorter type declaration for pairs that are pushed into map of FOP cards
#define c_pair std::pair<string,card*>

//! shorter type declaration for a map of string:(string:field) pairs
#define fields_map map<string,map<string,field*>>

//! shorter type declaration fo ra vector of field pointers (as in manager)
#define fields_vec vector<vector<field*>>

//! easier type declaration for a map of string:param_int4 pairs
#define int4_map map<string,param_int4>

//! easier type declaration for a map of string:param_int4_array pairs
#define int4arr_map map<string,param_int4_array>

//! easier type declaration for a map of string:param_real8 pairs
#define real8_map map<string,param_real8>

//! easier type declaration for a map of string:param_string pairs
#define str_map map<string,param_string>

//! easier type declaration for a map of string:real8 pairs
#define r8arr_map map<string,param_r8_array>

//! easier type declaration for a map of strings -> string vectors
#define str_vec_map map<string,vector<string>>
