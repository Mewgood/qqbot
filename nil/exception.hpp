#ifndef NIL_EXCEPTION_HPP
#define NIL_EXCEPTION_HPP

#include <stdexcept>

namespace nil
{
	class exception: std::exception
	{
	public:
		exception();
		exception(char const * message);

		char const * get_message() const;

	private:
		char const * message;
	};
}

#endif
