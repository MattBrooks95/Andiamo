#include "regex_manager.h"

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

	try {
		return regular_expressions.at(regex_pattern);
	} catch(out_of_range& not_found){
		return build_regular_expression(regex_pattern);
	}

}

regex* regex_manager::build_regular_expression(const string& regex_pattern){

	regex* new_regex = new regex(regex_pattern);
	regular_expressions.insert(pair<string,regex*>(regex_pattern,new_regex));
	return new_regex;

}
