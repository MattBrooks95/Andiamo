//! works similarly to the asset manager, lazy loads regular expression objects

#pragma once

#include<utility>
#include<map>
#include<regex>

class regex_manager{
	public:
		// regex_manager();
		//! deletes objects whose references are stored in regular_expressions
		~regex_manager();

		//! return regex keyed on by pattern, builds if needed
		std::regex* get_regular_expression(const std::string& pattern);

		//! builds a regex, and puts it in the regular_expressions map
		std::regex* build_regular_expression(const std::string& pattern);

	private:
		std::map<std::string,std::regex*> regular_expressions;
};
