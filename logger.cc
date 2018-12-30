//! \file logger.cc describes the functions and objects declared in logger.h

#include "logger.h"
using namespace std;

logger::logger(){

	//default to not worrying about operational messages
	verbose = false;

	//build dynamic file name ##################################################
	string path = system_access->get_home() + "/Andiamo/error_logs/";

	string prefix = "andiamo_errors_";//constant first part
	string suffix = ".txt";//constant file extension

	//store the date and time of day as a unique file name
	//to minimize over-writing
	string time_string;

	//unique_file_name will be comprised of a message and the date

	//http://www.cplusplus.com/reference/ctime/localtime/ 
	time_t unix_time;
	struct tm* time_info;
	time(&unix_time);
	time_info = localtime(&unix_time);
	
	string time_text = asctime(time_info);

	time_string = to_string(time_info->tm_hour) + "h_" 
					+ to_string(time_info->tm_min) + "m_" 
					+ to_string(time_info->tm_mon + 1) + "_" 
					+ to_string(time_info->tm_mday) + "_" 
					+ to_string(time_info->tm_year - 100);


	unique_file_name = path + prefix + time_string + suffix;

	//cout << unique_file_name << endl;
	//#########################################################################

	errors_out.open( unique_file_name.c_str() );
	//it didn't work, make the dir and try again
	if(errors_out.fail()){
		string mkdir_target  = system_access->get_home() + "/Andiamo/error_logs";
		string mkdir_command = "mkdir " + mkdir_target;
		system(mkdir_command.c_str());
		errors_out.open( unique_file_name.c_str() );
	//it worked
	} else {
		//close the file for now
		errors_out.close();
	}
	
}

logger::~logger(){
	//remove old files if there's too many files in the /error_logs directory
	cleaning_check();
	//output the error messages to the file
	make_error_file();
}


void logger::push_error(const std::string& push_me){
	errors_vector.push_back(push_me);//put message in the vector
	error_msg_num++;//increment the error counter

}

void logger::push_error(const string& push_1,const string& push_2){
	errors_vector.push_back(push_1);
	errors_vector.push_back(push_2);
	//the two strings are likely describing the same error,
	//so only increment once
	error_msg_num++;
}

void logger::cleaning_check(){

	vector<string> file_names;

	//this allows the opening of a directory as if it were a file
	DIR* dir_point;
	struct dirent *file_in_dir;
	string assets_path = system_access->get_home() + "/Andiamo/error_logs";
	dir_point = opendir(assets_path.c_str());
	if(dir_point != NULL){

		//readdir is kind of like a getline statement, read in info
		//then act on it
		while( (file_in_dir = readdir(dir_point)) ){

			//this ensures that only regular files are considered, and
			//not the . and .. directories that exist in nearly every
			//linux directory (but hidden)
			if( file_in_dir->d_type == DT_REG){

				//save # in vector
				file_names.push_back(file_in_dir->d_name);
			}
		}

		//close the directory
		closedir(dir_point);

	} else {
		string err;
		err  = "Failure to open the /error_logs file,";
		err += " for cleaning by cleaning_check()";
		push_error(err);
	}

	//if there's too many files in the folder, clean it out
	if(file_names.size() > 30){

		//this call sorts the vector of file names, with the most
		//recent files having the lowest index in the vector. This way,
		//we can pop_off the oldest entries and delete their files
		//until only 20 files exist
		sort(file_names.begin(),file_names.end(), file_compare);

		/*
		for(unsigned int c = 0; c < file_names.size() ; c++){
			cout << file_names[c] << endl;
		}*/

		while(file_names.size() > 19){
			//grab file name to complete path
			string doomed_one = system_access->get_home() + "/Andiamo/error_logs/" + file_names.back();
			//bash arg goes here
			string sys_command = "rm ";

			//poor lad, so full of life
			system((sys_command + doomed_one).c_str());
			file_names.pop_back();
		}

	//nothing to do if there is less than 20 files in the folder
	} else {
		return;
	}


}

void logger::push_msg(const std::string& push_me){
	//do nothing if we are not in verbose mode
	if(!verbose) return;
	//if we are in verbose mode, accumulate this message
	message_vector.push_back(push_me);

}

void logger::push_msg_no_nl(const std::string& push_me){
	//do nothing if we are not in verbose mode
	if(!verbose) return;
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

//############# non-class functions #############################//
bool file_compare(string str_one, string str_two){
	//http://www.cplusplus.com/reference/regex/regex_search/ used as reference

	//this matches the numbers in the file name
	regex split_nums("[0-9]+");

	//these are filled in by quantify_file_name, and then used to
	//compare the recentness of the files
	int str1_nums[5] = {0,0,0,0,0};
	int str2_nums[5] = {0,0,0,0,0};

	//cout << "#######################################################" << endl;
	quantify_file_name(split_nums,str_one,str1_nums);
	//cout << "##################" << endl;
	quantify_file_name(split_nums,str_two,str2_nums);
	//cout << "#######################################################" << endl;

	for(unsigned int c = 4; c > 0; c--){
		if(str1_nums[c] > str2_nums[c]) return true;
		else if(str1_nums[c] == str2_nums[c]) continue;
		else return false;
	}

	return false;
}

void quantify_file_name(const regex& regex_in,string str_in, int* numbers_in){

	smatch match;
	//fill array of time info for string one
	for(unsigned int c = 0; regex_search(str_in,match,regex_in);c++ ){
		numbers_in[c] = stoi(match.str());
		str_in = match.suffix().str();
	}

	/*
	for(unsigned int c = 0; c < 5;c++){
		cout << numbers_in[c] << endl;

	}*/

}







