#include "system_wrapper.h"

using namespace std;

system_wrapper::system_wrapper(){
	home_environment = getenv("HOME");

	#ifdef LINUX
	os = linux;
	cout << "LINUX" << endl;
	#endif

	#ifdef WINDOWS
	os = windows;
	cout << "WINDOWS" << endl;
	#endif

	#ifdef APPLE
	os = apple;
	cout << "APPLE" << endl;
	#endif

	save_start_time();

	cout << home_environment << endl;
}

string system_wrapper::get_home(){
	return home_environment;
}

string system_wrapper::get_andiamo_root(){
	return get_home() + ANDIAMO_FOLDER;
}

operating_system system_wrapper::get_os(){
	return os;
}

string system_wrapper::get_os_string(){
	if(os == linux) {
		return "Linux";
	} else if(os == windows) {
		return "Windows";
	} else if(os == apple) {
		return "Apple";
	} else {
		return "Undetermined!";
	}
}

void system_wrapper::print_os(){
	cout << "User operating system is: " << get_os_string() << endl;
}

void system_wrapper::save_start_time(){
	//unique_file_name will be comprised of a message and the date
	//http://www.cplusplus.com/reference/ctime/localtime/ 
	time_t unix_time;
	time(&unix_time);
	local_time_at_startup = localtime(&unix_time);
}

tm* system_wrapper::get_local_startup_time(){
	return local_time_at_startup;
}

std::string system_wrapper::get_local_startup_time_string(){
	return asctime(local_time_at_startup);
}

void system_wrapper::get_file_as_lines(const string& file_path,vector<string>& file_lines){
	cout << "in get file as lines, path:" << file_path << endl;
	fstream file_input_stream;
	file_input_stream.open(file_path.c_str());

	if (file_input_stream.fail()){
		logger_access->push_error("Couldn't get file as lines:" + file_path);
		file_input_stream.close();
		return;
	}

	string line;

	while(getline(file_input_stream,line)){
		cout << "pushing:" << line << endl;
		file_lines.push_back(line);
	}

	file_input_stream.close();
}

