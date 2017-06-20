#include<iostream>
#include<fstream>
#include<cstdlib>
using namespace std;


int main(){

	int dir_results = system("mkdir output");
	dir_results = system("touch output/output.txt");
	cout << dir_results << endl;
	fstream outs;
	outs.open("./output/output.txt");

	if(outs.fail()){
		cout << "It didn't work..." << endl;
	}
        outs << "There are countless ingredients that make up the human "
	     << "body and mind, like \n"
	     << "all components that make up me, as an individual.\n"
	     << "A face and voice to distinguish oneself\n"
             << "from others.The hand you see when you wake up.\n"
             << "Your childhood memories and feelings about your future.\n"
             << "And that's not all. There's also the ability to access\n"
             << "vast amounts of information from an infinite network.\n"
             << "All of that blends to create a mixture that forms me \n"
             << "and gives rise to my conscience. At the same time, though,\n"
             << "I feel continually confined within such boundaries." << endl;

	outs.close();
	return 0;//Exit success
}
