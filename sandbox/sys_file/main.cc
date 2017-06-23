#include<iostream>
#include<fstream>
#include<cstdlib>

using namespace std;

//this is a practise function sorts to see how parts of button and input_manager should work
int main(){

	ofstream outs;
	outs.open("./output/output.txt",std::fstream::trunc);
	if(outs.fail()){
		cout << "Out fstream failed." << endl;
		cout << "./output/ dir probably doesn't exist, creating it..." << endl;
		system("mkdir output");
		outs.open("./output/output.txt",std::fstream::trunc);
		if(!outs.fail()){
			cout << "Made ./output/ dir, seems to be fixed." << endl;
		} else {
			cout << "File stream still failed." << endl;
		}
	}
	//outs << "KONO DIO DA" << endl;
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
