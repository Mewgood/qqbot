#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <nil/environment.hpp>

#include <iostream>
#include <string>

#include <nil/string.hpp>

#define DEBUG_MARK std::cout << __FILE__ << " (line " << __LINE__ << ")" << std::endl;

#ifdef NIL_WINDOWS

#include <nil/windows.hpp>

namespace nil
{
	void debug_message(std::string const & message);
}

#endif

#endif
