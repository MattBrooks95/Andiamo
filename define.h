//! \file define.h declares some macros that replace long C++ object types
#pragma once

//! easier type declaration for an unsigned integer
#define UINT unsigned int

//! shorter type declaration for pairs that are pushed into map of FOP cards
#define C_PAIR std::pair<string,card*>

//! shorter type declaration for a map of string:(string:field) pairs
#define FIELDS_MAP map<string,map<string,field*>>

//! shorter type declaration fo ra vector of field pointers (as in manager)
#define FIELDS_VEC vector<vector<field*>>

#define LINE_TITLE LINE_GUIDE

//! shorter type declaration for storing line_guides in sdl_help
#define LINE_GUIDE std::pair<SDL_Rect,SDL_Texture*>

//! easier type declaration for a map of string:param_int4 pairs
#define INT4_MAP map<string,param_int4>

//! easier type declaration for a map of string:param_int4_array pairs
#define INT4_ARR_MAP map<string,param_int4_array>

//! easier type declaration for a map of string:param_real8 pairs
#define REAL8_MAP map<string,param_real8>

//! easier type declaration for a map of string:param_string pairs
#define STR_MAP map<string,param_string>

//! easier type declaration for a map of string:real8 pairs
#define R8_ARR_MAP map<string,param_r8_array>

//! easier type declaration for a map of strings -> string vectors
#define STR_VEC_MAP map<string,vector<string>>
