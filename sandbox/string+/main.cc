#include<iostream>
#include<vector>
#include "string+.h"
using namespace std;

int main(){
	string test = "This sentence should be split along spaces.";
	string test2 = "This_sentence_should_be_split_along_underscores.";
	string test3 = "This should be split along spaces, \"but this should be one item\"";

	vector<string> result1 = split(test,' ');
	vector<string> result2 = split(test2,'_');
	vector<string> result3 = split(test3,' ');

	for(unsigned int c = 0; c < result1.size();c++){
		cout << result1[c] << endl;
	}
	cout << "#######################################################################" << endl;
	for(unsigned int c = 0; c < result2.size();c++){
		cout << result2[c] << endl;
	}
	cout << "#######################################################################" << endl;
	for(unsigned int c = 0; c < result3.size();c++){
		cout << result3[c] << endl;
	}



}
