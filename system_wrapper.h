//! \file system.h object that wraps system functions
#pragma once

#include <iostream>
#include <string.h>

enum operating_system{
	linux,
	windows,
	apple
};

#ifdef __linux__
#define LINUX
class system_wrapper{

	public:
		system_wrapper();
		// ~system_wrapper();

		std::string get_home();

		operating_system get_os();
		std::string get_os_string(); 
		void print_os();

	private:
		std::string home_environment;
		operating_system os;
};
#elif __WIN32
#define WINDOWS
#elif __OSX__
#define APPLE
#endif
