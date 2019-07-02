//! \file system.h object that wraps system functions
#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "logger.h"

enum operating_system{
	linux,
	windows,
	apple
};

#define ANDIAMO_FOLDER "/Andiamo/"

#ifdef __linux__
#define LINUX
#elif __WIN32
#define WINDOWS
#elif __OSX__
#define APPLE
#endif

extern logger* logger_access;

class system_wrapper{
	public:
		std::string get_home();
		std::string get_andiamo_root();

		operating_system get_os();
		std::string get_os_string();
		void print_os();

		#ifdef LINUX
			system_wrapper();
			// ~system_wrapper();

			void save_start_time();

			void get_file_as_lines(const std::string& file_path, std::vector<std::string>& file_lines);

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
