//! \file deck.h implements cards for the FOP input
#pragma once

#include <vector>
#include <map>
#include <utility> //for pairs
#include <string>
#include <regex>

#include "logger.h"
#include "define.h"

extern logger error_logger;

using std::vector;
using std::string;

//forward declaration lets card class come after deck class
class card;

//! stores several card objects, represents a FOP input for one channel
class deck{

  public:
	//! crams default c,d,e,f,l,s,t,u,v cards into the vector of cards
	deck();

	//deck(const deck& other);

	//! cleans out the card pointers in the deck
	~deck();

  private:
	//! store pointers to cards and derived card objects
	std::map<string,card*> cards;
};



//! data structure to store lines (cards) for the FOP input
class card{

  public:

	//! constructor saves a string, and the number of params the card needs
	card(const string& letter_in = " ",const string& info_in = "",
							unsigned int num_params_in = 7);

	//! makes sure that the line is formatted properly
	/*! the default version of this function checks for
	 *num_params double values in the list. If they aren't all there
	 *something is wrong */ 
	virtual bool check();

  protected:

	//! character that tells what type of card this is
	string letter;

	//! comma-separated list of expected parameters
	string info;

	//! stores the number of parameters the line expects
	unsigned int num_params;

};

//! card object specialized to process the title card (just a string)
/*! note that a blank title card terminates reading of FOP input */
class title_card : public card{

  public:
    title_card(const string& letter_in,const string& info_in,
               uint num_params_in);
	//!make sure that the title card is formatted correctly
	/*! the only restrictions here are that the card is less
	 *than 80 columns, and does NOT start with a blank. A blank title
	 *card tells FOP to terminate */
	bool check();


  private:

};



