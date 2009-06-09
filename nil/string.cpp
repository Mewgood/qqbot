#include <nil/string.hpp>

namespace nil
{
	namespace string
	{
		bool is_alpha(char input)
		{
			return (input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z');
		}

		bool is_digit(char input)
		{
			return input >= '0' && input <= '9';
		}

		bool is_oct_digit(char input)
		{
			return input >= '0' && input <= '7';
		}

		bool is_hex_digit(char input)
		{
			return is_digit(input) || (input >= 'a' && input <= 'f') || (input >= 'A' && input <= 'F');
		}

		bool is_whitespace(char input)
		{
			return input == '\t' || input == '\n' || input == '\r' || input == ' ';
		}

		char to_upper(char input)
		{
			char output = input;
			if(output >= 'a' && output <= 'z')
				output -= 32;
			return output;
		}

		char to_lower(char input)
		{
			char output = input;
			if(output >= 'A' && output <= 'Z')
				output += 32;
			return output;
		}

		std::string to_upper(std::string const & input)
		{
			std::string output = input;
			for(std::string::iterator i = output.begin(), end = output.end(); i != end; ++i)
			{
				char & current_char = *i;
				current_char = to_upper(current_char);
			}
			return output;
		}

		std::string to_lower(std::string const & input)
		{
			std::string output = input;
			for(std::string::iterator i = output.begin(), end = output.end(); i != end; ++i)
			{
				char & current_char = *i;
				current_char = to_lower(current_char);
			}
			return output;
		}

		std::string left_trim(std::string const & input)
		{
			std::string output = input;
			std::string::iterator
				begin = output.begin(),
				end = output.end();

			for(;; ++begin)
			{
				if(begin >= end)
				{
					return "";
				}

				if(is_whitespace(*begin) == false)
				{
					break;
				}
			}

			output.erase(output.begin(), begin);

			return output;
		}

		std::string right_trim(std::string const & input)
		{
			std::string output = input;
			std::string::iterator
				end = output.end();

			for(end -= 1; is_whitespace(*end); --end);

			output.erase(end + 1, output.end());

			return output;
		}

		std::string trim(std::string const & input)
		{
			std::string output = input;
			std::string::iterator
				begin = output.begin(),
				end = output.end();

			for(;; ++begin)
			{
				if(begin >= end)
				{
					return "";
				}

				if(is_whitespace(*begin) == false)
				{
					break;
				}
			}

			for(end -= 1; is_whitespace(*end); --end);

			std::size_t offset_begin = begin - output.begin();
			std::size_t offset_end = end - output.begin();
			output = output.substr(offset_begin, offset_end - offset_begin + 1);

			return output;
		}

		unsigned int count(std::string const & input, std::string const & target)
		{
			if(input.empty() == true)
			{
				return 0;
			}

			if(target.empty() == true)
			{
				throw exception("Empty target");
			}

			unsigned int output = 0;

			std::size_t target_length = target.length();
			std::size_t result = input.find(target);

			while(result != std::string::npos)
			{
				++output;
				result = input.find(target, result + target_length);
			}

			return output;
		}

		std::string erase(std::string const & input, std::string const & target)
		{
			std::string output = input;

			if(target.empty())
			{
				throw exception("Empty target");
			}

			std::size_t target_length = target.length();

			std::size_t result = output.find(target);

			while(result != std::string::npos)
			{
				output.erase(result, target_length);
				result = output.find(target, result);
			}

			return output;
		}

		std::string replace(std::string const & input, std::string const & target, std::string const & replacement)
		{
			std::string output = input;

			if(target.empty() == true)
			{
				throw exception("Empty target");
			}

			if(target == replacement)
			{
				throw exception("Target and replacement are equal");
			}

			std::size_t target_length = target.length();
			std::size_t replacement_length = replacement.length();

			std::size_t result = output.find(target);

			while(result != std::string::npos)
			{
				output.replace(result, target_length, replacement);
				//result = output.find(target, result + replacement_length);
				result = output.find(target, result);
			}

			return output;
		}

		std::vector<std::string> tokenise(std::string const & input, char delimiter)
		{
			std::vector<std::string> output;
			std::string token;
			std::istringstream stream(input);
			while(std::getline(stream, token, delimiter))
					output.push_back(token);
			return output;
		}

		std::vector<std::string> tokenise(std::string const & input, std::string const & delimiter)
		{
			std::vector<std::string> output;
			std::size_t
				delimiter_length = delimiter.length(),
				left = 0,
				right = input.find(delimiter);
			while(right != std::string::npos)
			{
				std::string new_token(input.substr(left, right - left));
				output.push_back(new_token);
				left = right + delimiter_length;
				right = input.find(delimiter, left);
			}
			output.push_back(input.substr(left));
			return output;
		}

		std::string join(std::vector<std::string> const & input, std::string const & delimiter)
		{
			std::string output;
			std::vector<std::string>::const_iterator
				i = input.begin(),
				end = input.end();
			if(i != end)
			{
				output = *i;
				for(++i; i != end; ++i)
				{
					output += delimiter + *i;
				}
			}
			return output;
		}

		std::string bool_to_string(bool input)
		{
			return input ? "true" : "false";
		}

		bool wildcard_match(std::string const & target, std::string const & wildcard_string)
		{
			std::string::const_iterator target_iterator = target.begin(), target_end = target.end();
			for(std::string::const_iterator i = wildcard_string.begin(), end = wildcard_string.end(); i != end;	++i, ++target_iterator)
			{
				char current_char = *i;
				switch(current_char)
				{
					case '*':
					{
						++i;
						if(i == end)
							return true;
						char next_char = *i;
						while(true)
						{
							if(target_iterator == target_end)
								return false;
							if(*target_iterator == next_char)
								break;
							++target_iterator;
						}
						break;
					}

					case '?':
					{
						if(target_iterator == target_end)
							return false;
						break;
					}

					default:
					{
						if(target_iterator == target_end)
							return false;
						if(current_char != *target_iterator)
							return false;
						break;
					}
				}
			}
			return (target_iterator == target_end);
		}

		std::string consolify(std::string const & input)
		{
			std::string output = input;
			output = replace(output, "\b", "\\b");
			output = replace(output, "\t", "\\t");
			output = replace(output, "\n", "\\n");
			output = replace(output, "\v", "\\v");
			output = replace(output, "\f", "\\f");
			output = replace(output, "\r", "\\r");
			output = replace(output, "\x07", "\\x07");
			return output;
		}

		std::string binary(unsigned input)
		{
			std::string output;
			for(std::size_t i = sizeof(unsigned) * 8; i > 0; i--)
				output.push_back('0' + ((input >> (i - 1)) & 1));
			return output;
		}

		std::string binary(char const * input, std::size_t size)
		{
			std::string output;
			for(std::size_t i = 0; i < size; i++)
			{
				unsigned byte = static_cast<unsigned>(static_cast<unsigned char>(input[i]));
				for(std::size_t i = 8; i > 0; i--)
					output.push_back('0' + ((byte >> (i - 1)) & 1));
			}
			return output;
		}

		std::size_t locate_string(std::string const & input, std::string const & target, long occurence, std::size_t offset)
		{
			for(; occurence > 0; occurence--)
			{
				offset = input.find(target, offset + 1);
				if(offset == std::string::npos)
					break;
			}
			return offset;
		}

		bool extract_string(std::string const & input, std::string const & left_target, std::string const & right_target, long occurence, std::string & output)
		{
			std::size_t left_offset = locate_string(input, left_target, occurence, 0);
			if(left_offset == std::string::npos)
				return false;
			left_offset += left_target.length();
			std::size_t right_offset = input.find(right_target, left_offset);
			if(right_offset == std::string::npos)
				return false;
			output = input.substr(left_offset, right_offset - left_offset);
			return true;
		}

		bool split_string(std::string const & input, std::string const & delimiter, std::string & left_string, std::string & right_string)
		{
			std::size_t offset = input.find(delimiter);
			if(offset == std::string::npos)
			{
				left_string = input;
				right_string.clear();
				return false;
			}
			left_string = input.substr(0, offset);
			std::size_t right_offset = offset + delimiter.length();
			right_string = input.substr(right_offset);
			return true;
		}
	}
}

std::string operator+(char left, std::string const & right)
{
	std::string string;
	string += left;
	return string + right;
}

std::string operator+(char const * left, std::string const & right)
{
	return std::string(left) + right;
}
