#include <nil/console.hpp>

#ifdef NIL_WINDOWS

namespace nil
{
	namespace console
	{
		namespace
		{
			HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		}

		std::string colour(colour_type foreground, colour_type background)
		{
			::SetConsoleTextAttribute(handle, static_cast<WORD>((static_cast<ulong>(background) << 4) | static_cast<ulong>(foreground)));
			return "";
		}

		std::string colour(colour_type foreground)
		{
			return colour(foreground, black);
		}

		coordinate get_dimensions()
		{
			::CONSOLE_SCREEN_BUFFER_INFO console_info;
			if(::GetConsoleScreenBufferInfo(handle, &console_info) == 0)
				throw nil::exception("Failed to retrieve console dimensions");
			coordinate output;
			::COORD dimensions = console_info.dwMaximumWindowSize;
			output.x = static_cast<long>(dimensions.X);
			output.y = static_cast<long>(dimensions.Y);
			return output;
		}
	}
}

#endif
