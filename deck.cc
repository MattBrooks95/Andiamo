#include "deck.h"

using namespace std;
//######### DECK ##############################################################
deck::deck(){

	card_list.resize(10,NULL);

	card_list[0] = new card("",5);   //label card
	card_list[1] = new card("c",5);
	card_list[2] = new card("d",4);
	card_list[3] = new card("e",4);
	card_list[4] = new card("f",5);
	card_list[5] = new card("l",5);
	card_list[6] = new card("s",5);
	card_list[7] = new card("t",5); 
	card_list[8] = new card("u",5);
	card_list[9] = new card("v",5); 

}

//deck::deck(const deck& other){;}
deck::~deck(){

	for(unsigned int c = 0; c < card_list.size(); c++){

		delete card_list[c];

	}


}



//#############################################################################



//######### CARDS #############################################################

card::card(string info_in, unsigned int num_params_in){

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
	//regex match_pattern("[A-Z] ([0-9]+?.[0-9]*?,){1,6},[0-9]+?.[0-9]*?\\s*?");
	regex match_pattern("[A-Za-z] (-?[0-9]*?.[0-9]*?,){0,6}-?[0-9]+?.[0-9]*");

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
	regex numbers_pattern("-?[0-9]+.[0-9]*");

	
	regex_search(info,results,numbers_pattern);

	//make sure there's the correct number of matches
	unsigned int num_results = results.size();

	if( !(num_results > 0 && num_results < 8)){
		//return false, because too many or too few matches
		return false;
	}

	//if we make it here, we are good, return true
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
