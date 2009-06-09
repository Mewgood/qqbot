#ifndef NIL_FILE_HPP
#define NIL_FILE_HPP

#include <string>
#include <vector>

#include <cstdio>

#include <nil/exception.hpp>
#include <nil/memory.hpp>
#include <nil/types.hpp>

namespace nil
{
	enum file_mode 
	{
		file_mode_big_endian,
		file_mode_little_endian
	};

	class file
	{
	public:

		file();
		file(std::string const & file_name, bool create_file = false);
		~file();

		bool is_open() const;

		void set_mode(file_mode new_file_mode);

		void use_big_endian();
		void use_little_endian();

		bool open(std::string const & file_name, bool create_file = false);
		void close();

		std::size_t read(char * output, std::size_t bytes);
		std::string read(std::size_t bytes);
		ulong read_dword();
		ulong read_word();
		char read_byte();

		void write(char const * data, std::size_t bytes);
		void write(std::string const & data);
		void write_dword(ulong data);
		void write_word(ulong data);
		void write_byte(char data);

		std::size_t get_pointer() const;
		void set_pointer(std::size_t offset);
		void seek_end();
		std::size_t get_size();

		void skip(std::size_t offset);

	private:
		FILE * file_descriptor;
		file_mode mode;

		bool read_data(char * output, std::size_t bytes, std::size_t & bytes_read);
		void write_data(char const * output, std::size_t bytes);

		void write_bytes(ulong input, std::size_t size);
		ulong read_bytes(std::size_t size);

		void check_file_descriptor() const;

		void error_file_descriptor();
		void error_unknown();
	};

	bool read_file(std::string const & file_name, std::string & output);
	bool write_file(std::string const & file_name, std::string const & input);
	bool append_to_file(std::string const & file_name, std::string const & input);

	bool read_lines(std::string const & file_name, std::vector<std::string> & output);

	bool read_directory(std::string const & path, std::vector<std::string> & output);

	bool move_file(std::string const & file_name, std::string const & new_name);
	
	bool get_file_size(std::string const & path, ullong & output);
}

#endif
