#ifndef NIL_STRING_HPP
#define NIL_STRING_HPP

#include <string>
#include <sstream>
#include <vector>
#include <ios>

#include <cstring>

#include <nil/exception.hpp>

namespace nil
{
	namespace string
	{
		bool is_alpha(char input);
		bool is_digit(char input);
		bool is_oct_digit(char input);
		bool is_hex_digit(char input);
		bool is_whitespace(char input);

		char to_upper(char input);
		char to_lower(char input);

		std::string to_upper(std::string const & input);
		std::string to_lower(std::string const & input);

		std::string left_trim(std::string const & input);
		std::string right_trim(std::string const & input);
		std::string trim(std::string const & input);

		unsigned int count(std::string const & input, std::string const & target);
		std::string erase(std::string const & input, std::string const & target);
		std::string replace(std::string const & input, std::string const & target, std::string const & replacement);

		std::vector<std::string> tokenise(std::string const & input, char delimiter);
		std::vector<std::string> tokenise(std::string const & input, std::string const & delimiter);

		std::string join(std::vector<std::string> const & input, std::string const & delimiter);

		std::string bool_to_string(bool input);

		bool wildcard_match(std::string const & target, std::string const & wildcard_string);

		std::string consolify(std::string const & input);

		template <class number_type>
			number_type string_to_number(std::string const & input, std::ios_base::fmtflags base = std::ios_base::dec)
		{
			number_type output;
			std::stringstream stream(input);
			stream.setf(base, std::ios_base::basefield);
			if((stream >> output) == 0)
				throw exception("String to number conversion failed");
			return output;
		}

		template <class number_type>
			bool string_to_number(std::string const & input, number_type & output, std::ios_base::fmtflags base = std::ios_base::dec)
		{
			std::stringstream stream(input);
			stream.setf(base, std::ios_base::basefield);
			if((stream >> output) == 0)
				return false;
			return true;
		}

		template <class number_type>
			std::string number_to_string(number_type input, std::ios_base::fmtflags base = std::ios_base::dec)
		{
			std::string output;
			std::stringstream stream;
			stream.setf(base, std::ios_base::basefield);
			stream << input;
			stream >> output;
			return output;
		}

		std::string binary(unsigned input);
		std::string binary(char const * input, std::size_t size);

		std::size_t locate_string(std::string const & input, std::string const & target, long occurence, std::size_t offset = 0);
		bool extract_string(std::string const & input, std::string const & left_target, std::string const & right_target, long occurence, std::string & output);
		bool split_string(std::string const & input, std::string const & delimiter, std::string & left_string, std::string & right_string);
	}
}

std::string operator+(char left, std::string const & right);
std::string operator+(char const * left, std::string const & right);

#endif
