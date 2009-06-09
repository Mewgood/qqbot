#include <nil/ini.hpp>

//debugging
#include <iostream>

namespace nil
{
	ini::ini()
	{
	}

	ini::ini(std::string const & file_name)
	{
		load(file_name);
	}

	bool ini::load(std::string const & new_file_name)
	{
		file_name = new_file_name;

		values.clear();

		std::string input;
		bool success = read_file(file_name, input);
		if(success == false)
		{
			return false;
		}

		std::string section = "config";
		std::vector<std::string> lines = string::tokenise(input, '\n');
		for(std::vector<std::string>::iterator i = lines.begin(), end = lines.end(); i != end; ++i)
		{
			std::string line = string::trim(*i);

			bool is_empty = line.empty();

			if((is_empty == true) || ((is_empty == false) && (line[0] == ';')))
			{
				// Comment or blank line
				continue;
			}

			if (line[0] == '[') {
				// A new section.
				if (line[0] != '[' || *(line.end() - 1) != ']') {
					std::cout << "Invalid sections in INI file";
					throw exception("Section format is incorrect");
				}
				line.erase(line.begin());
				line.erase(line.end() - 1);
				section = line;
				continue;
			}

			std::size_t offset = line.find('=');
			if(offset == std::string::npos)
			{
				continue;
			}

			std::string
				variable = string::right_trim(line.substr(0, offset)),
				value = string::left_trim(line.substr(offset + 1));

			if((value.length() >= 2) && (value[0] == '"') && (*(value.end() - 1) == '"'))
			{
				value.erase(value.begin());
				value.erase(value.end() - 1);
			}

			//values[variable] = value;
			values[section][variable] = value;
		}

		return true;
	}

	bool ini::read_string(std::string const & section_name, std::string const & variable_name, std::string & output)
	{
		std::map<std::string, std::string>::iterator search = values[section_name].find(variable_name);
		if(search == values[section_name].end())
			return false;

		output = search->second;
		return true;
	}

	std::string ini::string(std::string const & section_name, std::string const & variable_name)
	{
		std::string output;
		bool success = read_string(section_name, variable_name, output);
		if(success == false)
		{
			std::cout << "Unable to find string value \"" << variable_name << "\" in \"" << file_name << "\"";
			throw exception("Missing value for required variable");
		}
		return output;
	}

	std::string ini::string(std::string const & section_name, std::string const & variable_name, std::string const & default_value)
	{
		std::string output;
		bool success = read_string(section_name, variable_name, output);
		if(success == false)
			return default_value;

		return output;
	}
}
