/*! \file \brief regex_patterns.h puts the various regular expression strings
 *in one place */
#pragma once

//######### used by the tile manager for finding defaults ################
#define RE_COMMENT_LINE "\\s*?#.*"
// #define RE_INITIAL_VALUE "(\\S*)\\s*?=\\s*?(\\S*)$"
#define RE_INITIAL_VALUE "(\\S*)\\s*?=\\s*?(.*)"
//########################################################################

//######## patterns used by the tile manager ###########
#define RE_IMG "\\s*?(.*\\.png)\\s*?"
#define RE_FIELD_SIZE "\\s*?([0-9]+?)\\s*?x\\s*?([0-9]+?)\\s*?"
//see if you can make these identify the line AND set up the proper capture groups
// #define RE_TILE_NAME "^([A-Za-z0-9_]+):?(.*)?"
#define RE_TILE_NAME "^([A-Za-z0-9_]+)(?::(.+))?"
#define RE_DESCRIPTION "^c (.*)"
#define RE_LINE_SEPARATOR "^(line_[0-9]+[A-Z]?)#*"

#define RE_ILV1 "\\s*6\\s*"
#define RE_ICNTRL4 "\\s*1\\s*"

#define RE_ICH4_UNLOCK "\\s*[1-6]\\s*"
#define RE_ICNTRL8_UNLOCK "\\s*[0-9]+?\\s*"
#define RE_ICNTRL10_UNLOCK "\\s*[0-9]+?\\s*"
#define RE_ILV3_ILV5_UNLOCK "\\s*[0-9]+?\\s*"
#define RE_ICNTRL6_UNLOCK "\\s*[12]\\s*"
#define RE_INM1_UNLOCK "\\s*[0-9]+\\s*"
//######################################################

//####### patterns used by text boxes to check inputs ##########
//integer
#define RE_INT5  "\\s*-?\\s*[0-9]{1,5}\\s*"

//reals
#define RE_F10_3 "\\s*-?\\s*[0-9]{1,6}\\s*\\.\\s*[0-9]{0,3}\\s*"
#define RE_F8_4  "\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,4}\\s*"
#define RE_F8_2  "\\s*-?\\s*[0-9]{1,6}\\s*\\.\\s*[0-9]{0,2}\\s*"
#define RE_F10_4 "\\s*-?\\s*[0-9]{1,5}\\s*\\.\\s*[0-9]{0,4}\\s*"
//not sure about this one, a floating point number with no digits
//after the decimal?
#define RE_F10_0 "\\s*-?\\s*[0-9]{1,10}\\s*\\.\\s*"
#define RE_F5_2  "\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,2}\\s*"
#define RE_F5_1  "\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,1}\\s*"
#define RE_F7_3  "\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,3}\\s*"
//######################################################

//####### patterns used by the form_button objects #####
#define RE_ICNTRL10_1 "(\\s*-?\\s*[0-9]{1,5}\\s*){1}\\s(\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,2}\\s*){1}"
#define RE_ICNTRL10_2 "(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"
#define RE_ICNTRL10_3 "(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"
//######################################################
