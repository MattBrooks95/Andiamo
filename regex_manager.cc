#include "regex_manager.h"
#include<iostream>
using namespace std;

// regex_manager::regex_manager(){
// }

regex_manager::~regex_manager(){
	for(map<string,regex*>::iterator it = regular_expressions.begin();
		it != regular_expressions.end();
		++it){
		delete(it->second);
	}
}

regex* regex_manager::get_regular_expression(const string& regex_pattern){
	cout << "making regex from pattern" << regex_pattern << endl;
	try {
		return regular_expressions.at(regex_pattern);
	} catch(out_of_range& not_found){
		return build_regular_expression(regex_pattern);
	}

}

regex* regex_manager::build_regular_expression(const string& regex_pattern){
	cout << "regex pattern:" << regex_pattern << endl;
	regex* new_regex = new regex(regex_pattern);
	regular_expressions.insert(pair<string,regex*>(regex_pattern,new_regex));
	return new_regex;

}
