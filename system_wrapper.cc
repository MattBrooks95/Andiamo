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
