#ifndef NIL_CONSOLE_HPP
#define NIL_CONSOLE_HPP

#include <nil/environment.hpp>
#include <nil/exception.hpp>
#include <nil/types.hpp>
#include <nil/graphics.hpp>

#ifdef NIL_WINDOWS

#include <string>
#include <nil/windows.hpp>

namespace nil
{
	namespace console
	{
		enum colour_type
		{
			black,
			dark_blue,
			dark_green,
			dark_cyan,
			dark_red,
			dark_pink,
			dark_yellow,
			light_grey,
			grey,
			blue,
			green,
			cyan,
			red,
			pink,
			yellow,
			white
		};

		colour_type const normal = light_grey;

		std::string colour(colour_type foreground, colour_type background);
		std::string colour(colour_type foreground);

		coordinate get_dimensions();
	}
}

#endif

#endif
