//! \file system.h object that wraps system functions
#pragma once

#include <cstdlib>
#include <iostream>
#include <string.h>

enum operating_system{
	linux,
	windows,
	apple
};

#ifdef __linux__
#define LINUX
#elif __WIN32
#define WINDOWS
#elif __OSX__
#define APPLE
#endif


class system_wrapper{

	public:

		std::string get_home();

		operating_system get_os();
		std::string get_os_string(); 
		void print_os();


		#ifdef LINUX
			system_wrapper();
			// ~system_wrapper();

			void save_start_time();

			tm* get_local_startup_time();
			std::string get_local_startup_time_string();
		#endif
		#ifdef WINDOWS
		#endif
		#ifdef APPLE
		#endif
	private:
		std::string home_environment;
		struct tm* local_time_at_startup;
		operating_system os;
};
