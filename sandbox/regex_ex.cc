// Example program
#include <iostream>
#include <string>
#include <regex>
#include <vector>

using namespace std;
int main()
{
	//set up regex matches
	regex re_comment("\\s*?#.*");
	regex int_four("\\s*?I4\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex int_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex char_array("\\s*?C\\*\\s*?[A-Za-z]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"");
	regex real_eight("\\s*?R8\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*?\\.[0-9]*?");

	vector<string> lines;	
	string comment = "#some comment here";
	string comment2 = "    #this one has spaces, and is more tricky";
	string string_ = "C* label|79| = \"Fe58(He-3,n)Fe58:HF2002 Calculations, Ed=10.0 MeV.\"";
	string real8_ = "R8 FNRMM1 = 1.0";
	string real8_2 = "R8 ROFL = 17.76";
	string int4_ = "I4 IENCH = 7";
	string int4_2 = "    I4        LMAO        =        123456789";
	string int_array_line = "I4(7) NENT = 20";

	lines.push_back(real8_2);
	lines.push_back(int4_2);
	lines.push_back(string_);
	lines.push_back(comment);
	lines.push_back(comment2);
	lines.push_back(real8_);
	lines.push_back(int4_);
	lines.push_back(int_array_line);

	for(unsigned int c = 0; c < lines.size();c++){
		bool found = false;
		cout << "Line: " << lines[c] << endl;
		if(regex_match(lines[c],int_four)){
			cout << "Is an int4 declaration line!\n" << endl;
			found = true;
		} if(regex_match(lines[c],real_eight)){
			cout << "Is a real 8 declaration line!\n" << endl;
			found = true;
		} if(regex_match(lines[c],int_array)){
			cout << "Is an int4 array declaration!\n" << endl;
			found = true;
		} if(regex_match(lines[c],re_comment)){
			cout << "Is a comment line! \n" << endl;
			found = true;
		} if(regex_match(lines[c],char_array)){
			cout << "Is a character array!\n" << endl;
			found = true;
		}
		if(!found){
			cout << "Wow, this line didn't hit any cases! You must "
		     	<< "be really bad at this!\n" << endl;
		}

	}


	regex int_array_size("\\([0-9]+?\\)");
	smatch size_match;

	string array_size_test_string = "I4(1776) SHREK = 2001";

	regex_search(array_size_test_string,size_match,int_array_size);

	if(size_match.ready()){
		string int_size_string = size_match[0].str();
		cout << int_size_string << endl;

	} else {
		cout << "Error! No matches." << endl;

	}



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
	/*
	string example = "c energy of projecticle in MeV in the lab\n";
    	regex desc("c[ ]+?.*\n");
	if(regex_match(example,desc)){
		cout << "Found the match!" << endl;
	} else cout << "Regex wasn't found. =( " << endl;
	*/
}

