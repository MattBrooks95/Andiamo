// Example program
#include <iostream>
#include <string>
#include <regex>
using namespace std;
int main()
{
/*
    string example = "HF2002";
    string example_2 = "HF2002.png";
    
    regex img(".*\\.png");
    regex name("[a-zA-Z0-9]+");
    if(regex_match(example,img)){
        cout << "Found background tile image name: " << example << endl;   
    }
    else if(regex_match(example,name)){
        cout << "Found background name: " << example << endl;   
    }
    
    if(regex_match(example_2,img)){
        cout << "Found background tile image name: " << example_2 << endl;
    }
    else if(regex_match(example_2,name)){
        cout << "Found background tile name: " << example_2 << endl;
    }
*/
	string example = "c energy of projecticle in MeV in the lab\n";
    	regex desc("c[ ]+?.*\n");
	if(regex_match(example,desc)){
		cout << "Found the match!" << endl;
	} else cout << "Regex wasn't found. =( " << endl;
}

