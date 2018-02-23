#pragma once

//much shorter
#define uint unsigned int

//shorthand reference to a map of a map string->parameter tile
#define fields_map map<string,map<string,field*>>

//used in manager::give_int4_fields_defaults
#define int4_map map<string,param_int4>

#define int4arr_map map<string,param_int4_array>

#define real8_map map<string,param_real8>

#define str_map map<string,param_string>

#define r8arr_map map<string,param_r8_array>

