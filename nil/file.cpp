#include <nil/file.hpp>

#include <nil/string.hpp>
#include <nil/environment.hpp>

#include <cstddef>

#ifdef NIL_WINDOWS
#define _WIN32_WINNT 0x500
#include <nil/windows.hpp>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include <iostream>

namespace nil
{
	file::file():
		file_descriptor(0),
		mode(file_mode_big_endian)
	{
	}

	file::file(std::string const & file_name, bool create_file):
		file_descriptor(0),
		mode(file_mode_big_endian)
	{
		open(file_name, create_file);
	}

	file::~file()
	{
		if(file_descriptor != 0)
			close();
	}

	void file::check_file_descriptor() const
	{
		if(file_descriptor == 0)
			throw exception("Missing file descriptor");
	}

	void file::error_unknown()
	{
		throw exception("Unknown error occured");
	}

	bool file::is_open() const
	{
		return file_descriptor != 0;
	}

	void file::set_mode(file_mode new_file_mode)
	{
		mode = new_file_mode;
	}

	void file::use_big_endian()
	{
		mode = file_mode_big_endian;
	}

	void file::use_little_endian()
	{
		mode = file_mode_little_endian;
	}

	bool file::read_data(char * output, std::size_t bytes, std::size_t & bytes_read)
	{
		check_file_descriptor();

		if(bytes == 0)
			return false;

		std::size_t result = std::fread(output, 1, bytes, file_descriptor);
		if(result == 0)
		{
			int fail = std::ferror(file_descriptor);
			if(fail != 0)
				error_unknown();
			bytes_read = 0;
			return true;
		}

		bytes_read = result;

		return false;
	}

	void file::write_data(char const * output, std::size_t bytes)
	{
		check_file_descriptor();

		if(bytes == 0)
			return;

		std::size_t success = std::fwrite(output, 1, bytes, file_descriptor);
		if(success == 0)
		{
			int fail = std::ferror(file_descriptor);
			if(fail != 0)
				error_unknown();
		}
	}

	void file::write_bytes(ulong input, std::size_t size)
	{
		check_file_descriptor();

		char output[4];

		switch(mode)
		{
			case file_mode_big_endian:
				write_big_endian(input, output, size);
				break;

			case file_mode_little_endian:
				write_little_endian(input, output, size);
				break;

			default:
				throw exception("Encountered an unknown file mode while trying to write to a file");
		}

		write_data(output, size);
	}

	ulong file::read_bytes(std::size_t size)
	{
		check_file_descriptor();

		char data[4];
		std::size_t bytes_read;
		read_data(data, size, bytes_read);

		ulong output;

		switch(mode)
		{
			case file_mode_big_endian:
				output = read_big_endian(data, size);
				break;

			case file_mode_little_endian:
				output = read_little_endian(data, size);
				break;

			default:
				throw exception("Encountered an unknown file mode while trying to read from a file");
		}
		return output;
	}

	bool file::open(std::string const & file_name, bool create_file)
	{
		if(file_descriptor)
			close();

		char fopen_mode[] = "w+b";
		if(!create_file)
			fopen_mode[0] = 'r';

		file_descriptor = std::fopen(file_name.c_str(), fopen_mode);

		return (file_descriptor != 0);
	}

	void file::close()
	{
		check_file_descriptor();
		std::fclose(file_descriptor);
	}

	std::size_t file::read(char * buffer, std::size_t bytes)
	{
		std::size_t output;
		read_data(buffer, bytes, output);
		return output;
	}

	std::string file::read(std::size_t bytes)
	{
		char * buffer = new char[bytes];
		std::size_t bytes_read;
		read_data(buffer, bytes, bytes_read);
		std::string output;
		output.assign(buffer, bytes_read);
		return output;
	}

	ulong file::read_dword()
	{
		return read_bytes(4);
	}

	ulong file::read_word()
	{
		return read_bytes(2);
	}

	char file::read_byte()
	{
		char output;
		std::size_t unused;
		read_data(&output, 1, unused);
		return output;
	}

	void file::write(char const * data, std::size_t bytes)
	{
		write_data(data, bytes);
	}

	void file::write(std::string const & data)
	{
		write_data(data.c_str(), data.length());
	}

	void file::write_dword(ulong data)
	{
		write_bytes(data, 4);
	}

	void file::write_word(ulong data)
	{
		write_bytes(data, 2);
	}

	void file::write_byte(char data)
	{
		write_data(&data, 1);
	}

	std::size_t file::get_size()
	{
		check_file_descriptor();
		fpos_t position;
		int fail = std::fgetpos(file_descriptor, &position);
		if(fail != 0)
			throw exception("Failed to retrieve file pointer");
		seek_end();
		std::size_t output = static_cast<std::size_t>(std::ftell(file_descriptor));
		fail = std::fsetpos(file_descriptor, &position);
		if(fail != 0)
			throw exception("Failed to set file pointer");
		return output;
	}

	std::size_t file::get_pointer() const
	{
		check_file_descriptor();
		long position = std::ftell(file_descriptor);
		if(position == -1)
			throw exception("Failed to retrieve file pointer");
		return static_cast<std::size_t>(position);
	}

	void file::set_pointer(std::size_t offset)
	{
		check_file_descriptor();
		int fail = std::fseek(file_descriptor, static_cast<long>(offset), SEEK_SET);
		if(fail != 0)
			throw exception("Failed to set file pointer");
	}

	void file::seek_end()
	{
		check_file_descriptor();
		int fail = std::fseek(file_descriptor, 0, SEEK_END);
		if(fail != 0)
			throw exception("Failed to seek end");
	}

	void file::skip(std::size_t offset)
	{
		set_pointer(get_pointer() + offset);
	}

	bool read_file(std::string const & file_name, std::string & output)
	{
		file file_object;
		bool success = file_object.open(file_name);
		if(!success)
			return false;
		std::size_t file_size = file_object.get_size();
		char * file_content = new char[file_size];
		file_object.read(file_content, file_size);
		output.assign(file_content, file_size);
		delete [] file_content;
		return true;
	}

	bool write_file(std::string const & file_name, std::string const & input)
	{
		file file_object;
		bool success = file_object.open(file_name, true);
		if(success == false)
			return false;
		file_object.write(input);
		return true;
	}
	
	bool append_to_file(std::string const & file_name, std::string const & input)
	{
		file file_object;
		bool success = file_object.open(file_name, true);
		if(success == false)
			return false;
		file_object.seek_end();
		file_object.write(input);
		return true;
	}

	bool read_lines(std::string const & file_name, std::vector<std::string> & output)
	{
		std::string input;
		bool result = read_file(file_name, input);
		if(result == false)
			return false;
		input = string::erase(input, "\r");
		output = string::tokenise(input, '\n');
		return true;
	}

	void add_directory_entry(std::vector<std::string> & files, std::string const & path, std::string const & file)
	{
		if(file != "." && file != "..")
			files.push_back(path + file);
	}

	bool read_directory(std::string const & path, std::vector<std::string> & output)
	{
#ifdef NIL_WINDOWS
		WIN32_FIND_DATA find_data;
		HANDLE find_handle = FindFirstFile((path + "*").c_str(), &find_data);
		if(find_handle == INVALID_HANDLE_VALUE)
		{
			FindClose(find_handle);
			return false;
		}
		add_directory_entry(output, path, find_data.cFileName);
		while(FindNextFile(find_handle, &find_data))
				add_directory_entry(output, path, find_data.cFileName);
		FindClose(find_handle);
		return true;
#else
		DIR * directory = ::opendir(path.c_str());
		if(directory == 0)
			return false;
		
		while(true)
		{
			dirent * directory_entity = ::readdir(directory);
			if(directory_entity == 0)
				break;
			add_directory_entry(output, path, directory_entity->d_name);
		}
		closedir(directory);
#endif
	}

	bool move_file(std::string const & file_name, std::string const & new_name)
	{
#ifdef NIL_WINDOWS
		return ::MoveFile(file_name.c_str(), new_name.c_str()) != 0;
#else
		return std::rename(file_name.c_str(), new_name.c_str()) != -1;
#endif
	}
	
	bool get_file_size(std::string const & path, ullong & output)
	{
#ifdef NIL_WINDOWS
		/*
		LPCTSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile
		*/
		HANDLE file_handle = ::CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(file_handle == INVALID_HANDLE_VALUE)
			return false;
		LARGE_INTEGER size;
		if(::GetFileSizeEx(file_handle, &size) == 0)
			return false;
		output = static_cast<ullong>(size.QuadPart);
		return true;
#else
		struct ::stat64 stats;
		int result = ::lstat64(path.c_str(), &stats);
		if(result == -1)
			return false;
		output = static_cast<ullong>(stats.st_size);
		return true;
#endif
	}
}
