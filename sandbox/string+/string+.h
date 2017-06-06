#include<string>
#include<vector>
std::vector<std::string> split(std::string split_me = "", char delim = ' ');

/* old version
	std::vector<std::string> return_me;
	if(split_me.empty()) return return_me;

	int index = 0;
	char temp = split_me[index];

	std::string push_me;
	
	while( index < split_me.size() ){ // go until end of string is reached
		push_me = "";//reset new word container

		while(temp == delim && index < split_me.size()){
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		while(temp != delim && index < split_me.size() ){
			push_me.push_back(temp);
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		std::cout << "Pushing: |" << push_me << "|" << std::endl;
		return_me.push_back(push_me);
	}

	return return_me;

*/

