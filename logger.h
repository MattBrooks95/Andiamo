#pragma once

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<regex>//for breaking file names up into integers

#include<cstdlib> //for system calls
#include<time.h>  //for time functions used to name files

#include<dirent.h>//allows traversing a directory and getting its file names. ./error_logs in this case
#include<algorithm>//std::sort, to sort a vector of file names

//! the logger is a class that provides a means by which error messages can be printed to a file
/*! I made this to reduce the terminal clutter involved with the many cout statements that I use(d) for debugging.
 *It has two primary functionalities: extensive debugging output and error message output. Caught exceptions or
 *shaky occurences within the logics of if-else statements are the kind of messages that wind up here. The output
 *from SDL_GetError() also winds up here often. It currently does not clean out the folder in which it stores the reports
 *, which is a feature I'd like to add sometime. It also only outputs debugging messages if -v is appended to its
 *terminal command, like ./andiamo -v */
class logger{
  public:
	//! constructor creates the error output stream (errors_out)
	/*! it checks to see if it actually opens or not. Also, if the 'logs' directory
	 *does not exist, it creates it with system calls */
	logger();
	//! the destructor calls make_error_file()
	~logger();

	//! puts a new error message into the vector
	/* these messages are accumulated and then printed to a file at the end of this object's life.
	 *This function also does some book keeping, like the # of error messages pushed */
	void push_error(std::string push_me);

	//! this is an overload of logger::push_error
	/*! it taks two strings as arguments, to allow long messages to be split up into parts for readability
	 * and brevity */
	void push_error(std::string push_1,std::string push_2);

	//! this function uses dirent functions to check the # of files, and then removes the oldest ones
	/*! If the # of files in the /error_logs directory is greater than 30, it culls the 10 oldest
	 * entries */
	void cleaning_check();

	//! This function accumulates messages about what happens during run time
	/* if verbose mode is turned on by the -v argument, these are accumulated and printed.
	 *if that argument is not present, this does nothing */
	void push_msg(std::string push_me);

	//! This functions pushes a message into the vector, but without a newline character
	void push_msg_no_nl(std::string push_me);

	//! this function creates the error file from the errors_vector
	/*! it is called in this class's destructor, so that it doesn't have to be called in main */
	void make_error_file();

	bool verbose;

  private:
	int error_msg_num;//!< keep track of the number of error messages pushed
	std::string unique_file_name;//!< save the file name that is based on the time in which Andiamo was ran

	std::vector<std::string> message_vector;//!< keep track of process outputs
	std::vector<std::string> errors_vector;//!< store up error messages, so they are all written at the end
	std::ofstream errors_out;//!< the stream that the error files will eventually be pushed to
	

};
//! this function is passed to std::sort, to allow it to sort file names by their recentness
bool file_compare(std::string str_one, std::string str_two);
//! this function is a helper for file_compare, which "tokenizes" the dates from the file name
void quantify_file_name(const std::regex& regex_in, std::string string_in, int* numbers_in);

