#ifndef NIL_INI_HPP
#define NIL_INI_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <nil/file.hpp>
#include <nil/string.hpp>
#include <nil/exception.hpp>


namespace nil
{
	class ini
	{

	public:

		ini();
		ini(std::string const & file_name);

		bool load(std::string const & new_file_name);
		//std::map<std::string, pickit_type> load_pickit(std::string const & new_file_name);

		std::string string(std::string const & section_name, std::string const & variable_name);
		std::string string(std::string const & section_name, std::string const & variable_name, std::string const & default_value);

		template <typename number_type>
			bool read_number(std::string const &variable_name, number_type &output)
		{
			std::map<std::string, std::string>::iterator search = values.find(variable_name);
			/*if(search == values.end())
			{
				return false;
			}*/

			try
			{
				output = string::string_to_number<number_type>(search->second);
			}
			catch(std::exception const &)
			{
				std::cout << "Unable to find string value \"" << variable_name << "\" in \"" << file_name << "\"";
				throw exception("Failed to parse numeric value for a variable");
			}

			return true;
		}

		template <typename number_type>
			number_type number(std::string const & variable_name)
		{
			number_type output;
			bool success = read_number<number_type>(variable_name, output);
			if(success == false)
			{
				std::cout << "Unable to find string value \"" << variable_name << "\" in \"" << file_name << "\"";
				throw exception("Missing value for a required variable");
			}
			return output;
		}

		template <typename number_type>
			number_type number(std::string const & variable_name, number_type default_value)
		{
			number_type output;
			bool success;

			try
			{
				success = read_number<number_type>(variable_name, output);
			}
			catch(std::exception const &)
			{
				return default_value;
			}

			if(success == false)
			{
				return default_value;
			}

			return output;
		}

	private:

		bool read_string(std::string const & section_name, std::string const & variable_name, std::string & output);

		std::string file_name;
		//std::map<std::string, std::string> values;

		// Confusing to read, but it's not too bad. It's essentially:
		// values[section][variable]
		std::map<std::string, std::map<std::string, std::string> > values;
	};
}

#endif
