#ifndef NIL_NET_HPP
#define NIL_NET_HPP

#include <string>
#include <vector>
#include <queue>
#include <map>

#include <nil/environment.hpp>
#include <nil/thread.hpp>
#include <nil/exception.hpp>
#include <nil/types.hpp>

#ifdef NIL_WINDOWS
#include <nil/windows.hpp>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#ifdef NIL_WITH_SSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

namespace nil
{
	namespace net
	{
		enum client_status_type
		{
			client_status_idle,
			client_status_lookup,
			client_status_connecting,
			client_status_connected
		};

		enum client_event_type
		{
			client_event_connect,
			client_event_disconnect,
			client_event_receive,
			client_event_error
		};

		enum client_error_type
		{
			client_error_socket,
			client_error_blocking_mode,
			client_error_dns,
			client_error_connect,
			client_error_receive,
			client_error_send
		};

		enum tcp_server_event_type
		{
			tcp_server_event_error,
			tcp_server_event_connect,
			tcp_server_event_disconnect,
			tcp_server_event_receive
		};

		enum tcp_server_error_type
		{
			tcp_server_error_socket,
			tcp_server_error_bind,
			tcp_server_error_listen,
			tcp_server_error_accept,
			tcp_server_error_receive
		};
		
#ifdef NIL_WINDOWS

		class winsock
		{
		public:
			winsock();
			~winsock();

			void launch();
			
			bool success() const;

		private:
			bool launched;
			bool successful_startup;
			nil::mutex mutex;
		};
#endif

#ifdef NIL_WITH_SSL

		class ssl_launcher_class
		{
		public:
			ssl_launcher_class();
			~ssl_launcher_class();

			void launch();

		private:
			bool launched;
			nil::mutex mutex;
		};

#endif

		struct client_event
		{
			client_event_type type;
			client_error_type error;
			std::string data;

			client_event();
			client_event(client_event_type type);
			client_event(client_error_type error);
			client_event(std::string const & input);
		};
		
#ifdef NIL_WINDOWS
		typedef SOCKET socket_type;
#else
		typedef int socket_type;
#endif

		class tcp_socket
		{
		public:
			tcp_socket();
			~tcp_socket();

			bool connect(ulong address, ulong port);
			bool connect(std::string const & address, ulong port);
			void disconnect();
			bool send(std::string const & input);
			bool send(void const * input, std::size_t input_size);
			bool receive(std::string & output);

			client_status_type get_status() const;

			bool get_ip(ulong & output);

		private:
			client_status_type status;
			socket_type socket_descriptor;
		};

		class tcp_client
		{
		public:
			tcp_client();
			~tcp_client();

			void connect(ulong address, ulong port);
			void connect(std::string const & address, ulong port);
			void disconnect();
			void send(std::string const & input);
			client_event get_event();

			client_status_type get_status() const;

			void wtf();

		private:
			thread client_thread;
			mutex client_mutex;
			client_status_type status;
			std::queue<client_event> event_queue;
			bool terminate_thread;

			socket_type socket_descriptor;
			event tcp_client_event;

			ulong address_number;
			std::string address_string;
			ulong address_port;

			void thread_function(void * unused);

			void push_event(client_event the_event);
			void error_event(client_error_type error);
		};

#ifdef NIL_WITH_SSL

		class ssl_tcp_socket
		{
		public:
			ssl_tcp_socket();
			~ssl_tcp_socket();

			bool connect(ulong address, ulong port);
			bool connect(std::string const & address, ulong port);
			void disconnect();
			bool send(std::string const & input);
			bool send(void const * input, std::size_t input_size);
			bool receive(std::string & output);

			client_status_type get_status() const;

			bool get_ip(ulong & output);

		private:
			client_status_type status;
			socket_type socket_descriptor;
			::SSL_CTX * ctx;
			::SSL * ssl;
		};

#endif

		struct tcp_server_event
		{
			tcp_server_event_type type;

			tcp_server_error_type error;
			int client_identifier;

			std::string data;

			tcp_server_event();
			tcp_server_event(tcp_server_event_type type);
			tcp_server_event(tcp_server_error_type error);
			tcp_server_event(ulong client_identifier, tcp_server_event_type type);
			tcp_server_event(ulong client_identifier, std::string data);
			tcp_server_event(ulong client_identifier, tcp_server_error_type error);
		};

		struct tcp_server_client
		{
			thread client_thread;
			socket_type client_socket;
			ulong identifier;
			std::string send_buffer;
			bool to_disconnect;
			ulong ip;

			tcp_server_client();
		};

		class tcp_server
		{
		public:
			tcp_server();
			~tcp_server();

			bool listen(ulong server_port);
			void shutdown();

			void send(ulong client_identifier, std::string const & data);
			void disconnect(ulong client_identifier);
			std::string get_ip(ulong client_identifier);

			tcp_server_event get_event();

		private:
			typedef std::vector<tcp_server_event> queue_type;
			typedef std::map<ulong, tcp_server_client *> map_type;
			typedef std::vector<ulong> dead_client_type;

			bool running;
			ulong port;
			thread server_thread;
			mutex server_mutex;
			event queue_event;
			bool terminate_thread;
			queue_type event_queue;
			map_type client_map;
			dead_client_type dead_clients;
			socket_type server_socket;

			void check_identifier(ulong identifier);
			bool is_valid_identifier(ulong identifier);

			void cleanup();

			void error_event(tcp_server_error_type error);
			void push_event(tcp_server_event server_event);

			ulong get_unused_id();

			void thread_function(void * unused);
			void client_thread(void * client_void_pointer);
		};

		class udp_client
		{
		public:
			udp_client();
			~udp_client();

			void set_target(ulong target_address, ulong target_port);
			bool set_target(std::string const & target_address, ulong target_port);

			void send(std::string const & data);
			std::string receive();

		private:
			bool target_is_set;
			sockaddr_in address;
			socket_type socket_descriptor;
		};

		struct udp_server_event
		{
			std::string data;
			ulong ip;
		};

		class udp_server
		{
		public:
			udp_server();
			~udp_server();

			bool launch(ulong port);
			void shut_down();
			udp_server_event receive();

		private:
			bool is_running;
			socket_type socket_descriptor;
		};

		ulong get_ip();
		std::string ip_to_string(ulong address);

#ifdef NIL_WINDOWS
		extern winsock winsock_launcher;
#endif
	}
}

#endif
