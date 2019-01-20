/*! \file \brief regex_patterns.h puts the various regular expression strings
 *in one place */
#pragma once

//######### patterns used by input_maker for init()################
#define RE_COMMENT_LINE "\\s*?#.*"
#define RE_INT4_LINE "\\s*?I4\\s+?[A-Za-z0-9_]+?\\s+?=.*"
#define RE_INT4_ARRAY "\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?-?[0-9]*?\\s*?,?)+?\"\\s*"
#define RE_STRING_LINE "\\s*?C\\*\\s*?[A-Za-z_]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"\\s*"
// #define RE_REAL8 "\\s*?R8\\s+?[A-Za-z0-9_]+?\\s+?=\\s+?((-?[0-9]*?\\.[0-9]*?)|(nodef))\\s*"
#define RE_REAL8_LINE "\\s*?R8\\s+?[A-Za-z0-9_]+?\\s+?=.*"
#define RE_FORM_INIT_LINE "FORM:[A-Za-z0-9_]*?\\s+?(-?[0-9]+?\\.?[0-9]*?[, |]?)*"
#define RE_STRING_ARRAY_SIZE "\\|\\d+?\\|"
#define RE_INT_ARRAY_SIZE "\\([0-9]+?\\)"
#define RE_R8_ARRAY_LINE "\\s*?R8\\(\\s*[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?[0-9]+?\\.[0-9]+?,?)+?\""
#define RE_NAD_FLAG "nodef"
//######################################################

//######## patterns used by the tile manager ###########
#define RE_IMG "\\s*?.*\\.png\\s*?"
#define RE_FIELD_SIZE "\\s*?[0-9]+?\\s*?x\\s*?[0-9]+?\\s*?"
#define RE_TILE_NAME "\\s*?([a-z0-9_A-Z]+?):?(.*)?\\s*?"
#define RE_SEMI ":"
#define RE_DESCRIPTION "c .*"
#define RE_LINE_SEPARATOR "\\s*?line_[0-9]+?[A-Z]?.*"

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
#define RE_INT5  "\\s*-?\\s*[0-9]{1,5}\\s*"
#define RE_F10_3 "\\s*-?\\s*[0-9]{1,6}\\s*\\.\\s*[0-9]{0,3}\\s*"
#define RE_F8_4  "\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,4}\\s*"
#define RE_F10_4 "\\s*-?\\s*[0-9]{1,5}\\s*\\.\\s*[0-9]{0,4}\\s*"
#define RE_F5_2  "\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,2}\\s*"
#define RE_F5_1  "\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,1}\\s*"
#define RE_F7_3  "\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,3}\\s*"
//######################################################

//####### patterns used by the form_button objects #####
#define RE_ICNTRL10_1 "(\\s*-?\\s*[0-9]{1,5}\\s*){1}\\s(\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,2}\\s*){1}"
#define RE_ICNTRL10_2 "(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"
#define RE_ICNTRL10_3 "(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"
//######################################################
