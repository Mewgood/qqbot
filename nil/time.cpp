#include <nil/time.hpp>

#include <ctime>
#include <sstream>

#include <nil/environment.hpp>

#ifdef NIL_WINDOWS
#include <nil/windows.hpp>
#else
#include <sys/time.h>
#include <sys/times.h>
#endif


#include <iostream>

namespace nil
{
	ulong time()
	{
		return static_cast<ulong>(std::time(0));
	}

	ullong milliseconds()
	{
#ifdef NIL_WINDOWS
		::SYSTEMTIME system_time;
		::GetLocalTime(&system_time);
		ullong output = time() * 1000ull + system_time.wMilliseconds;
#else
		ullong output = time() * 1000ull;
		::timeval time_value;
		::gettimeofday(&time_value, 0);
		::tm * time_data = ::localtime(&time_value.tv_sec);
		output += static_cast<ullong>(time_value.tv_usec / 1000);
#endif
		return output;
	}

	std::string timestamp()
	{
		std::time_t current_time;
		std::time(&current_time);
		std::tm * pointer = std::localtime(&current_time);
		std::stringstream output;
		output.fill('0');
		output << (pointer->tm_year + 1900) << ".";
		output.width(2);
		output << (pointer->tm_mon + 1) << ".";
		output.width(2);
		output << pointer->tm_mday << " ";
		output.width(2);
		output << pointer->tm_hour << ":";
		output.width(2);
		output << pointer->tm_min << ":";
		output.width(2);
		output << pointer->tm_sec;
		return output.str();
	}

	
	ullong boot_time()
	{
#ifdef NIL_WINDOWS
#if (_WIN32_WINNT >= 0x0600)
		return static_cast<ullong>(::GetTickCount64());
#else
		return static_cast<ullong>(::GetTickCount());
#endif
#else
		tms tm;
		return static_cast<ullong>(::times(&tm));
#endif
	}
}
