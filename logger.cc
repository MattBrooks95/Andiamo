//! \file logger.cc describes the functions and objects declared in logger.h

#include "logger.h"

#include "system_wrapper.h"

using namespace std;

logger::logger(){

	//default to not worrying about operational messages
	verbose = false;

	string home_path = system_access->get_home();
	string path_to_logs = "/Andiamo/error_logs/";

	error_logs_root_directory = home_path + path_to_logs;

	unique_file_name = get_unique_log_name();

	errors_out.open( unique_file_name.c_str() );
	//it didn't work, make the dir and try again
	if(errors_out.fail()){
		string mkdir_command = "mkdir " + error_logs_root_directory;
		system(mkdir_command.c_str());
		errors_out.open( unique_file_name.c_str() );
	//it worked
	} else {
		//close the file for now
		errors_out.close();
	}

	string local_time = system_access->get_local_startup_time_string();
	string time_message = "Local time at start:" + local_time;
	push_msg(time_message);
}

logger::~logger(){
	//remove old files if there's too many files in the /error_logs directory
	cleaning_check();
	//output the error messages to the file
	make_log_file();
}

string logger::get_unique_log_name(){

	string path = error_logs_root_directory;

	string prefix = "andiamo_errors_";
	//constant file extension
	string extension = ".txt";

	tm* time_info = system_access->get_local_startup_time();

	string hours   = to_string(time_info->tm_hour) + "h_";
	string minutes = to_string(time_info->tm_min) + "m_";
	string day     = to_string(time_info->tm_mon + 1) + "_"; 
	string month   = to_string(time_info->tm_mday) + "_";
	string year    = to_string(time_info->tm_year - 100);

	string time_string = hours + minutes + day + month + year;

	return path + prefix + time_string + extension;
}

void logger::push_error(const std::string& push_me){
	errors_vector.push_back(push_me);
	error_msg_num++;
}

void logger::push_error(const string& push_1,const string& push_2){
	errors_vector.push_back(push_1);
	errors_vector.push_back(push_2);
	//the two strings are likely describing the same error, so only increment once
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
		string err = "Failure to open the /error_logs file,";
		err       += " for cleaning by cleaning_check()";
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

void logger::make_log_file(){
	string bars = "#############";

	errors_out.open(unique_file_name);
	if (verbose) {
		errors_out << bars << " Messages " << bars << endl;
		print_log_vector(errors_out,message_vector);
	}

	errors_out << "\n" << bars << " Errors " << bars << endl;
	print_log_vector(errors_out,errors_vector);
}

void logger::print_log_vector(ofstream& output_stream, vector<string>& print_me){
		for(unsigned int c = 0; c < print_me.size();c++){
			output_stream << print_me[c] << "\n";
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







