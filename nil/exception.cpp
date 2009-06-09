#include <nil/exception.hpp>

namespace nil
{
	exception::exception()
	{
	}

	exception::exception(char const * message):
		message(message)
	{
	}

	char const * exception::get_message() const
	{
		return message;
	}
}
