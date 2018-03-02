//! \file deck.cc implements the functions declared in deck.h

#include "deck.h"

using namespace std;
//######### DECK ##############################################################
deck::deck(){

	card_list.resize(10,NULL);

	card_list[0] = new card("","Default label",5);   //label card
	card_list[1] = new card("C","",5);
	card_list[2] = new card("D","",4);
	card_list[3] = new card("E","",4);

	//we're not fitting data, 0 out F card
	card_list[4] = new card("F","0.0,0.0,0.0,0,0.0",5);

	//card L indicates that we want to calculate TCs, so 0,3
	card_list[5] = new card("L","0,3",5);

	//the following cards come from our data directory
	card_list[6] = new card("S","",5);
	card_list[7] = new card("T","",5); 
	card_list[8] = new card("U","",5);
	card_list[9] = new card("V","",5); 
}

//deck::deck(const deck& other){;}
deck::~deck(){

	for(unsigned int c = 0; c < card_list.size(); c++){

		delete card_list[c];

	}
}

//#############################################################################



//######### CARDS #############################################################

card::card(const string& letter_in,const string& info_in,
												unsigned int num_params_in){

	letter     = letter_in;
	info       = info_in;
	num_params = num_params_in;

}

bool card::check(){

	if(info.size() > 80){
		error_logger.push_error("Card too long! Exceeds 80 columns.");
		return false;
	}

	//make sure that the line is formatted properly
	//#############################################
	//regex match_pattern("[A-Za-z] (-?[0-9]*?.?[0-9]*?,){0,6}-?[0-9]*?.[0-9]*");
	regex match_pattern("[A-Za-z] (-?[0-9]*.?[0-9]*,){0,6}-?[0-9]*.?[0-9]*");

	//check the integrety pattern against the line stored in this object
	//storing each number's match in results
	if(!regex_match(info,match_pattern)){
		return false;
	}

	//makes sure that the values themselves are of proper form
	//########################################################

	//store the numbers themselves, as ripped from the line
	smatch results;

	//pattern to make sure they're floats
	//regex numbers_pattern("-?[0-9]+.[0-9]*");

	//number pattern to count # of entries, make sure it lines up with
	//num_params
	regex numbers_pattern("-?[0-9*.?[0-9]*");
	
	regex_search(info,results,numbers_pattern);

	//make sure there's the correct number of matches
	unsigned int num_results = results.size();

	if( !(num_results > 0 && num_results < num_params)){
		//return false, because too many or too few matches
		return false;
	}

	//if we make it here, we are good, return true
	return true;
}

bool title_card::check(){

	//can't start with a blank
	if(info[0] == ' ') return false;

	//can't be longer than 80 columns
	if(info.size() > 80) return false;

	//if we make it here, everything is good
	return true;
}

/* an example FOP input from Zach
12C + 180 calculations Conteaud Nuclear Physics A250 p 182
c 10.,30.0,0.5,0.0,0.0
d 6.0,12.0,0.0,0.
e 8.0,18.,-.7826,0.0
f 0.0,0.0,0.0,0,0.0
l 0,3
s 100.,27,0,0,0
t 0.0,0.0,0.0
u 2.226,2.379,0.0,0.0,0.0,1.35
v 0.48,0.26,0.0,0.0
*/
