//! \file field.cc implements the functions declared in field.h
#include "field.h"

using namespace std;

field::field(string tile_name_in,string image_name_in, int width, int height){
	tile_name = tile_name_in;
	image_name = image_name_in;

	size.width = width;
        size.height = height;
}

void field::print(ostream& outs){
	outs << "Tile name: " << tile_name << " Tile Image Name: "
	     << image_name << endl;
	size.print(outs);
	cout << "\n\n" << endl;
}



field::~field(){

}
