#include <nil/debug.hpp>
#include <nil/windows.hpp>

#ifdef NIL_WINDOWS

namespace nil
{
	void debug_message(std::string const & message)
	{
		OutputDebugString((message + "\n").c_str());
	}
}

#endif
