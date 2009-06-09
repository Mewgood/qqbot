#ifndef NIL_HTTP_HPP
#define NIL_HTTP_HPP

#include <string>

#include <nil/time.hpp>
#include <nil/net.hpp>
#include <nil/function.hpp>

namespace nil
{
	class http_file_download
	{
	public:
		typedef nil::unary_function<void, http_file_download &> function_type;

		http_file_download(std::string const & url, std::string const & file);
		bool download_started();
		bool error_occured();
		std::string get_progress();
		double get_speed_value();
		std::string get_speed();
		void set_download_handler(function_type const & new_download_handler);
		std::string get_file() const;

	private:
		ullong download_begin;
		std::string host, path, file;
		ulong port;
		nil::thread download_thread;
		net::tcp_socket http_socket;
		bool started, error;
		ullong bytes_downloaded, file_size;
		function_type download_handler;
		bool got_download_handler;

		void perform_download(void * unused);
		void run_download_handler();
	};
}

#endif
