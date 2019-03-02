//! \file std_helpers.h has some convenience functions for handling exceptions thrown by std containers
#pragma once

#include<iostream>
#include<string>
#include<map>

using std::string;
using std::map;
using std::vector;


template <class any_type>
any_type get_map_entry(const map<string,any_type>& map,const string& key,std::ostream& output_stream){
	try {
		return map.at(key);
	} catch(std::out_of_range& not_found){
		output_stream << not_found.what() << std::endl;
		any_type junk;
		return junk;
	}
}

template <class any_type>
any_type get_map_entry(const map<string,any_type>& map,const string& key,vector<string>& message_log){
	try {
		return map.at(key);
	} catch(std::out_of_range& not_found){
		message_log.push_back(not_found.what());
		any_type junk;
		return junk;
	}
}