#include <nil/http.hpp>

#include <nil/string.hpp>
#include <nil/types.hpp>
#include <nil/file.hpp>

#include <sstream>

namespace nil
{
	void parse_url(std::string url, std::string & host, std::string & path, ulong & port)
	{
		url = nil::string::replace(url, "http://", "");
		std::size_t offset = url.find('/');
		if(offset == std::string::npos)
		{
			host = url;
			path = "/";
		}
		else
		{
			host = url.substr(0, offset);
			path = url.substr(offset);
		}
		offset = host.find(':');
		if(offset == std::string::npos)
			port = 80;
		else
		{
			std::string port_string = host.substr(offset + 1);
			if(!nil::string::string_to_number<ulong>(port_string, port))
				port = 80;
		}
	}

	long extract_size(std::string const & header)
	{
		string_vector lines = nil::string::tokenise(header, "\r\n");
		for(string_vector::iterator i = lines.begin(), end = lines.end(); i != end; i++)
		{
			std::string const & line = *i;
			string_vector tokens = nil::string::tokenise(line, " ");
			if(tokens.size() != 2)
				continue;
			if(tokens[0] == "Content-Length:")
				return nil::string::string_to_number<long>(tokens[1]);
		}
		throw nil::exception("Invalid Content-Length specified in HTTP header");
	}

	http_file_download::http_file_download(std::string const & url, std::string const & file):
		file(file),
		started(false),
		error(false),
		got_download_handler(false)
	{
		parse_url(url, host, path, port);
		download_begin = nil::milliseconds();
		download_thread.start(nil::thread::function_type(*this, &http_file_download::perform_download));
	}

	bool http_file_download::download_started()
	{
		return started;
	}

	bool http_file_download::error_occured()
	{
		return error;
	}

	std::string http_file_download::get_progress()
	{
		std::stringstream stream;
		stream.precision(2);
		stream << std::fixed << (static_cast<double>(bytes_downloaded) / file_size * 100.0) << '%';
		return stream.str();
	}

	double http_file_download::get_speed_value()
	{
		ullong difference = nil::milliseconds() - download_begin;

		//hack lol
		if(difference == 0ull)
			difference = 1;

		return bytes_downloaded / 1024.0 / difference * 1000.0;
	}

	std::string http_file_download::get_speed()
	{
		double speed = get_speed_value();
		std::stringstream stream;
		stream.precision(2);
		stream << std::fixed << speed << " KiB/s";
		return stream.str();
	}

	void http_file_download::set_download_handler(function_type const & new_download_handler)
	{
		download_handler = new_download_handler;
		got_download_handler = true;
	}

	std::string http_file_download::get_file() const
	{
		return file;
	}

	void  http_file_download::perform_download(void * unused)
	{
		nil::net::tcp_socket socket;
		if(!socket.connect(host, port))
		{
			error = true;
			run_download_handler();
			return;
		}
		std::string packet = "GET " + path + " HTTP/1.1\r\n";
		packet += "Host: " + host + "\r\n";
		packet += "Connection: Close\r\n\r\n";
		socket.send(packet);

		nil::file output_file;
		output_file.open(file, true);

		std::string head;
		std::string const target = "\r\n\r\n";

		while(true)
		{
			std::string data;
			if(!socket.receive(data))
				break;
			head += data;
			std::size_t offset = head.find(target);
			if(offset != std::string::npos)
			{
				offset += target.length();
				std::string data = head.substr(offset);
				head = head.substr(0, offset);
				try
				{
					file_size = extract_size(head);
				}
				catch(nil::exception &)
				{
					error = true;

					run_download_handler();
					return;
				}
				output_file.write(data);
				bytes_downloaded = static_cast<ullong>(data.size());
				break;
			}
		}

		download_begin = nil::milliseconds();
		started = true;

		bool run = true;
		while(run)
		{
			std::string data;
			run = socket.receive(data);
			output_file.write(data);
			bytes_downloaded += static_cast<ullong>(data.size());
		}

		if(bytes_downloaded < file_size)
			error = true;

		output_file.close();

		run_download_handler();
	}

	void http_file_download::run_download_handler()
	{
		if(got_download_handler)
			download_handler(*this);
	}
}
