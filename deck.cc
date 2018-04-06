//! \file deck.cc implements the functions declared in deck.h

#include "deck.h"

using namespace std;
//######### DECK ##############################################################
deck::deck(){

    //label card
	cards.insert(c_pair("title",new title_card("","Default label",5)));
    cards.insert(c_pair("C",new card("C","",5)));
    cards.insert(c_pair("D",new card("D","",4)));
    cards.insert(c_pair("E",new card("E","",4)));

	//we're not fitting data, 0 out F card
    cards.insert(c_pair("F",new card("F","0.0,0.0,0.0,0,0.0",5)));

    //card L says we need transmission coefficients, so 0,3
    cards.insert(c_pair("L",new card("L","0,3",5)));

    //cards are created using the data directory
    cards.insert(c_pair("S",new card("S","",5)));
    cards.insert(c_pair("T",new card("T","",5)));
    cards.insert(c_pair("U",new card("U","",5)));
    cards.insert(c_pair("V",new card("V","",5)));
}

//deck::deck(const deck& other){;}
deck::~deck(){

	for(map<string,card*>::iterator it = cards.begin();
        it != cards.end();
        it++){

        if(it->second != NULL){
		    delete it->second;
        }

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

title_card::title_card(const string& letter_in,const string& info_in,
                       uint num_params_in){

	letter     = letter_in;
	info       = info_in;
	num_params = num_params_in;

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
