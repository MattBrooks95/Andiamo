#include "logger.h"
using namespace std;

logger::logger(){


	verbose = false;//default to not worrying about operational messages

	//build dynamic file name ###########################################################################
	string path = "./error_logs/";
	string prefix = "andiamo_errors_";//constant first part
	string suffix = ".txt";//constant file extension
	string time_string;//store the date and time of day as a unique file name to minimize over writing

	//unique_file_name will be comprised of a message and the date

	time_t unix_time; //http://www.cplusplus.com/reference/ctime/localtime/ 
	struct tm* time_info;
	time(&unix_time);
	time_info = localtime(&unix_time);


	time_string = to_string(time_info->tm_hour) + "h_" + to_string(time_info->tm_min) + "m_" +
		      to_string(time_info->tm_mon) + "_" + to_string(time_info->tm_mday) + "_" + 
		      to_string(time_info->tm_year - 100);


	unique_file_name = path + prefix + time_string + suffix;

	//cout << unique_file_name << endl;
	//####################################################################################################

	errors_out.open( unique_file_name.c_str() );
	if(errors_out.fail()){ //it didn't work, make the dir and try again
		system("mkdir error_logs");
		errors_out.open( unique_file_name.c_str() );
	} else { //it worked
		errors_out.close();//close the file for now
	}
	
}

logger::~logger(){
	make_error_file();//output the error messages to the file

}


void logger::push_error(std::string push_me){
	errors_vector.push_back(push_me);//put message in the vector
	error_msg_num++;//increment the error counter

}
void logger::push_msg(std::string push_me){
	if(!verbose) return;//do nothing if we are not in verbose mode
	message_vector.push_back(push_me);//if we are in verbose mode, accumulate this message

}

void logger::push_msg_no_nl(std::string push_me){
	if(!verbose) return;//do nothing if we are not in verbose mode
	message_vector.back().append(push_me);
}

void logger::make_error_file(){

	errors_out.open(unique_file_name);
	errors_out << "#############Verbose messages###################" << endl;
	if(verbose){
		for(unsigned int c = 0; c < message_vector.size();c++){
			errors_out << message_vector[c] << "\n";

		}
	}

	errors_out << "\n##################Error messages##############" << endl;
	for(unsigned int c = 0; c < errors_vector.size();c++){
		errors_out << errors_vector[c] << "\n";
	}

}

