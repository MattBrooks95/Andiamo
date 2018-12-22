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

	cout << home_environment << endl;
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