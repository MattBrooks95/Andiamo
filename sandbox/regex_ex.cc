// Example program
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <fstream>

using namespace std;

class pattern{
  public:
	pattern(const string& pattern, const string& intent){
		my_pattern = pattern;
		my_intent  = intent;
		my_regex = regex(pattern);
	}
	regex get_re(){
		return my_regex;
	}

	string get_str(){
		return my_pattern + " with intent " + my_intent;
	}

  private:
	regex  my_regex;
	string my_intent;
	string my_pattern;
};

int main(){

	ofstream outs;
	outs.open("test_results.txt");

	//two integers with field width 5, one float with field width 10, precision 3
	pattern int5("\\s*[0-9]{0,5}\\s*","int5");
	pattern f10_3("\\s*[0-9]{1,6}\\.[0-9]{0,3}\\s*","f10_3");
	pattern f8_4 ("\\s*[0-9]{1,4}\\.[0-9]{0,4}\\s*","f8_4");
	pattern f10_4("\\s*[0-9]{1,5}\\.[0-9]{0,4}\\s*","f10_4");
	pattern f5_2 ("\\s*[0-9]{1,3}\\.[0-9]{0,2}\\s*","f5_2");
	pattern f7_3 ("\\s*[0-9]{1,4}\\.[0-9]{0,3}\\s*","f7_3");


	vector<pattern> pattern_list;
	pattern_list.push_back(int5);
	pattern_list.push_back(f10_3);
	pattern_list.push_back(f8_4);
	pattern_list.push_back(f7_3);

	vector<string> test_lines = {"45", " 45 ", " 4a6", "4.56", "40000", "matthew", "123.31",
								 "john", "13..45", ".205", "100000.342", "12345678910111213",
								 "11511.72934812", "mark", "123.456", "lu.ke", "19.",
								 "kusanagi.motoko", "79.052", "2501", "17..76", "19.95",
								 "01.2.34", "mega...deth", "1842.295.", ".1393.45.223"};


	for(unsigned int c = 0; c < pattern_list.size(); c++){
		outs << "#########################################################################\n";
		outs << "TEST: " << pattern_list[c].get_str() << endl;
		outs << "#########################################################################\n";
		for(unsigned int d = 0; d < test_lines.size(); d++){
			if(regex_match(test_lines[d],pattern_list[c].get_re())){
				outs << "Passes: " << "|" << test_lines[d] << "|" << endl;
			} else {
				outs << "Fails : " << "|" << test_lines[d] << "|" << endl;
			}
		}
		outs << "#########################################################################\n";
	}

/*
	//set up regex matches
	regex re_comment("\\s*?#.*");
	regex int_four("\\s*?I4\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");

	//regex int_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex int_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9]+?\\s*?=\\s*?\"(\\s*?[0-9]*?\\s*?,?)+?\"");
	//#########################################################################^
	regex char_array("\\s*?C\\*\\s*?[A-Za-z]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"");
	regex real_eight("\\s*?R8\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*?\\.[0-9]*?");

	regex e_array("\\s*?E\\(\\s*[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9]+?\\s*?=\\s*?\"(\\s*?[0-9]+?\\.[0-9]+?,?)+?\"");


	vector<string> lines;	
	string comment = "#some comment here";
	string comment2 = "    #this one has spaces, and is more tricky";
	string string_ = "C* label|79| = \"Fe58(He-3,n)Fe58:HF2002 Calculations, Ed=10.0 MeV.\"";
	string real8_ = "R8 FNRMM1 = 1.0";
	string real8_2 = "R8 ROFL = 17.76";
	string int4_ = "I4 IENCH = 7";
	string int4_2 = "    I4        LMAO        =        123456789";
	string e_example = "E(8) TIN = \"0.831,0.803,0.742,0.630,0.460,0.290,0.140,0.061,0.022,0.007\"";

	string int_array_line = "I4(7) NENT = \"20,20,0,0,0,20\"";
	//string int_array_line = "I4(7) NENT = \"20\"";

	lines.push_back(real8_2);
	lines.push_back(int4_2);
	lines.push_back(string_);
	lines.push_back(comment);
	lines.push_back(comment2);
	lines.push_back(real8_);
	lines.push_back(int4_);
	lines.push_back(int_array_line);
	lines.push_back(e_example);

	for(unsigned int c = 0; c < lines.size();c++){
		bool found = false;
		outs << "Line: " << lines[c] << endl;
		if(regex_match(lines[c],int_four)){
			outs << "Is an int4 declaration line!\n" << endl;
			found = true;
		} if(regex_match(lines[c],real_eight)){
			outs << "Is a real 8 declaration line!\n" << endl;
			found = true;
		} if(regex_match(lines[c],int_array)){
			outs << "Is an int4 array declaration!\n" << endl;
			found = true;
		} if(regex_match(lines[c],re_comment)){
			outs << "Is a comment line! \n" << endl;
			found = true;
		} if(regex_match(lines[c],char_array)){
			outs << "Is a character array!\n" << endl;
			found = true;
		} if( regex_match(lines[c],e_array) ){
			outs << "Is an array of E!" << endl;
			found = true;

		}
		if(!found){
			outs << "Wow, this line didn't hit any cases! You must "
		     	<< "be really bad at this!\n" << endl;
		}

	}
*/
	/*
	regex int_array_size("\\([0-9]+?\\)");
	smatch size_match;

	string array_size_test_string = "I4(1776) SHREK = 2001";

	regex_search(array_size_test_string,size_match,int_array_size);

	if(size_match.ready()){
		string int_size_string = size_match[0].str();
		outs << int_size_string << endl;

	} else {
		outs << "Error! No matches." << endl;

	}*/



/*
    string example = "HF2002";
    string example_2 = "HF2002.png";
    
    regex img(".*\\.png");
    regex name("[a-zA-Z0-9]+");
    if(regex_match(example,img)){
        outs << "Found background tile image name: " << example << endl;   
    }
    else if(regex_match(example,name)){
        outs << "Found background name: " << example << endl;   
    }
    
    if(regex_match(example_2,img)){
        outs << "Found background tile image name: " << example_2 << endl;
    }
    else if(regex_match(example_2,name)){
        outs << "Found background tile name: " << example_2 << endl;
    }
*/
	/*
	string example = "c energy of projecticle in MeV in the lab\n";
    	regex desc("c[ ]+?.*\n");
	if(regex_match(example,desc)){
		outs << "Found the match!" << endl;
	} else outs << "Regex wasn't found. =( " << endl;
	*/
	outs.flush();
	outs.close();
}

