#include <iostream>
#include "ftran_structs.h"
using namespace std;


int main(){
	param_real8 gits("Motoko",1989);
	param_real8 akira("Kaneda", 1988);
	//cout << "real8: " << gits.name << " " << gits.value << endl;

	//cout << (gits == akira) << endl;
	cout << "Gits: " << gits << " Akira: " << akira << endl;
	gits = akira;
	cout << "Gits after \"gits = akira\": " << gits << endl;

	param_real8 one_piece("Luffy");
	cout << one_piece << endl;
	one_piece = 1.776;
	cout << one_piece << endl;
	one_piece = "Nami";
	cout << one_piece << endl;

	cout << "one_piece == \"Nami\": " << (one_piece == "Nami") << endl;
	cout << "one_piece == \"Elsa\": " << (one_piece == "Elsa") << endl;
	cout << "one_piece == 1.776: " << (one_piece == 1.776) << endl;
	cout << "one_piece == 1995: " << (one_piece == 1995) << endl;
	int no = 42;
	one_piece = no;
	cout << "one_piece after \"one_piece = no (int=42);\" :" << one_piece << endl;

	param_int4 haruhi;
	haruhi = 43.0;
	haruhi = "SOS Brigade";
	cout << haruhi << endl;
	haruhi = 43;
	cout << haruhi << endl;




	return 0; //EXIT SUCCESS
}
