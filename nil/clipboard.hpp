#ifndef NIL_CLIPBOARD_HPP
#define NIL_CLIPBOARD_HPP

#include <nil/environment.hpp>

#ifdef NIL_WINDOWS

#include <string>

#include <nil/exception.hpp>

namespace nil
{
	std::string get_clipboard();
	void set_clipboard(std::string const & input);
}

#endif

#endif
