#include<iostream>
#include<string>
#include<map>

using namespace std;


int main(){

	map<string,int> test_map;


	test_map.insert( pair<string,int>("Matthew",1995) );
	test_map.insert( pair<string,int>("Caitlin",2012) );

	test_map.insert( pair<string,int>("Ghost in the Shell",1989) );


	cout << test_map.at("Matthew") << endl;


	cout << test_map.at("Ghost in the Shell") << endl;

	map<string,int>::iterator test_i = test_map.begin();

	for(test_i; test_i != test_map.end(); test_i++){
		cout << test_i->second << endl;
	}

	return 0;//EXIT SUCCESS
}
