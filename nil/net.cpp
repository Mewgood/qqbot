#include <nil/net.hpp>

#include <nil/memory.hpp>
#include <nil/thread.hpp>
#include <nil/debug.hpp>

#include <iostream>
#include <sstream>

namespace nil
{
	namespace net
	{
		std::size_t const buffer_size = 4096;

#ifdef NIL_WINDOWS
		typedef int socket_address_size_type;
		typedef u_long socket_address_type;
#else
		typedef socklen_t socket_address_size_type;
		typedef in_addr_t socket_address_type;
#endif

#ifdef NIL_WINDOWS
		socket_type const invalid_socket_value = INVALID_SOCKET;
		int const invalid_connect_value = SOCKET_ERROR;
		int const invalid_receive_value = SOCKET_ERROR;
		int const invalid_send_value = SOCKET_ERROR;
		int const invalid_socket_name_value = SOCKET_ERROR;
		int const invalid_bind_value = SOCKET_ERROR;
#else
		socket_type const invalid_socket_value = -1;
		int const invalid_connect_value = -1;
		int const invalid_receive_value = -1;
		int const invalid_send_value = -1;
		int const invalid_socket_name_value = -1;
		int const invalid_bind_value = -1;
#endif

		void close_socket(socket_type socket_descriptor)
		{
#ifdef NIL_WINDOWS
			::closesocket(socket_descriptor);
#else
			::shutdown(socket_descriptor, SHUT_RDWR);
#endif
		}

		socket_address_type & extract_address(sockaddr_in & address)
		{
#ifdef NIL_WINDOWS
			return address.sin_addr.S_un.S_addr;
#else
			return address.sin_addr.s_addr;
#endif
		}

#ifdef NIL_WINDOWS
		winsock winsock_launcher;

		winsock::winsock():
			launched(false)
		{
		}

		winsock::~winsock()
		{
			if(launched && successful_startup)
				::WSACleanup();
		}

		void winsock::launch()
		{
			mutex.lock();
			if(!launched)
			{
				::WSADATA wsa_data;
				int error = ::WSAStartup(MAKEWORD(2, 2), &wsa_data);
				launched = true;
				successful_startup = (error == 0);
			}
			mutex.unlock();
		}

		bool winsock::success() const
		{
			return successful_startup;
		}
#endif

#ifdef NIL_WITH_SSL

		ssl_launcher_class ssl_launcher;

		ssl_launcher_class::ssl_launcher_class():
			launched(false)
		{
		}

		ssl_launcher_class::~ssl_launcher_class()
		{
		}

		void ssl_launcher_class::launch()
		{
			mutex.lock();
			if(!launched)
			{
				::SSL_load_error_strings();
				::SSL_library_init();
				launched = true;
			}
			mutex.unlock();
		}

#endif

		void launch_winsock()
		{
#ifdef NIL_WINDOWS
			winsock_launcher.launch();
#endif
		}

		void launch_ssl()
		{
#ifdef NIL_WITH_SSL
			ssl_launcher.launch();
#endif
		}

		client_event::client_event(client_event_type type):
			type(type)
		{
		}

		client_event::client_event(client_error_type error):
			type(client_event_error),
			error(error)
		{
		}

		client_event::client_event(std::string const & input):
			type(client_event_receive),
			data(input)
		{
		}

		tcp_socket::tcp_socket():
			status(client_status_idle)
		{
			launch_winsock();
		}

		tcp_socket::~tcp_socket()
		{
			disconnect();
		}

		bool tcp_socket::connect(ulong address, ulong port)
		{
			//std::cout << "WSAGetLastError:" << WSAGetLastError() << std::endl;
			disconnect();
			socket_descriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(socket_descriptor == invalid_socket_value)
			{
				//std::cout << "WSAGetLastError:" << WSAGetLastError() << std::endl;
				//std::cout << winsock_launcher.success() << std::endl;
				throw nil::exception("Unable to create TCP socket");
			}

			sockaddr_in server_address;
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = static_cast<u_long>(address);
			server_address.sin_port = ::htons(static_cast<u_short>(port));

			status = client_status_connecting;
			int result = ::connect(socket_descriptor, reinterpret_cast<sockaddr *>(&server_address), static_cast<int>(sizeof(sockaddr_in)));

			if(result == invalid_connect_value)
				return false;

			status = client_status_connected;
			return true;
		}

		bool tcp_socket::connect(std::string const & address, ulong port)
		{
			ulong result = ::inet_addr(address.c_str());
			ulong address_number;
			if(result == INADDR_NONE)
			{
				status = client_status_lookup;
				::hostent * host_entity = ::gethostbyname(address.c_str());
				if(host_entity == 0)
					return false;
				address_number = *reinterpret_cast<ulong *>(host_entity->h_addr);
			}
			else
				address_number = result;
			return connect(address_number, port);
		}

		void tcp_socket::disconnect()
		{
			if(status != client_status_idle && status != client_status_lookup)
			{
				close_socket(socket_descriptor);
				status = client_status_idle;
			}
		}

		bool tcp_socket::send(std::string const & input)
		{
			return send(input.c_str(), input.length());
		}

		bool tcp_socket::send(void const * input, std::size_t input_size)
		{
			if(status == client_status_connected)
			{
				int result = ::send(socket_descriptor, reinterpret_cast<const char *>(input), static_cast<int>(input_size), 0);
				if(result == invalid_send_value)
				{
					status = client_status_idle;
					return false;
				}
				return true;
			}
			return false;
		}

		bool tcp_socket::receive(std::string & output)
		{
			if(status != client_status_connected)
				return false;
			char buffer[buffer_size];
			int result = ::recv(socket_descriptor, buffer, buffer_size, 0);
			if(result == 0 || result == invalid_receive_value)
			{
				status = client_status_idle;
				return false;
			}
			output.assign(buffer, static_cast<std::size_t>(result));
			return true;
		}

		client_status_type tcp_socket::get_status() const
		{
			return status;
		}

		bool tcp_socket::get_ip(ulong & output)
		{
			if(status != client_status_connected)
				return false;
			sockaddr_in socket_address;
			socket_address_size_type socket_address_size = static_cast<socket_address_size_type>(sizeof(socket_address));
			int result = ::getsockname(socket_descriptor, reinterpret_cast<sockaddr *>(&socket_address), &socket_address_size);
			if(result == invalid_socket_name_value)
				return false;
			output = static_cast<ulong>(extract_address(socket_address));
			return true;
		}

		tcp_client::tcp_client():
			status(client_status_idle),
			terminate_thread(false)
		{
			launch_winsock();
		}

		tcp_client::~tcp_client()
		{
			disconnect();
		}

		void tcp_client::wtf()
		{
		}

		void tcp_client::connect(ulong address, ulong port)
		{
			disconnect();
			address_number = address;
			address_string.clear();
			address_port = port;
			client_thread.start(thread::function_type(*this, &tcp_client::thread_function));
		}

		void tcp_client::connect(std::string const & address, ulong port)
		{
			client_mutex.info();
			disconnect();
			address_string = address;
			address_port = port;
			client_thread.start(thread::function_type(*this, &tcp_client::thread_function));
		}

		void tcp_client::disconnect()
		{
			if(status != client_status_idle)
			{
				terminate_thread = true;
				client_thread.join();
				terminate_thread = false;
				status = client_status_idle;
			}
		}

		void tcp_client::send(std::string const & input)
		{
			if(status == client_status_connected)
			{
				int result = ::send(socket_descriptor, input.c_str(), static_cast<int>(input.length()), 0);
				if(result == invalid_send_value)
					throw exception("Failed to send data");
			}
		}

		client_event tcp_client::get_event()
		{
			/*
			tcp_client_event.wait();
			client_mutex.lock();
			client_event output = event_queue.front();
			event_queue.pop();
			if(event_queue.empty() == true)
				tcp_client_event.reset();
			client_mutex.unlock();
			return output;
			*/
			client_mutex.lock();
			std::size_t queue_size = event_queue.size();
			if(event_queue.empty())
			{
				client_mutex.unlock();
				tcp_client_event.wait();
				client_mutex.lock();
			}
			client_event output = event_queue.front();
			client_mutex.unlock();
			return output;
		}

		client_status_type tcp_client::get_status() const
		{
			return status;
		}

		void tcp_client::push_event(client_event the_event)
		{
			client_mutex.lock();
			event_queue.push(the_event);
			tcp_client_event.set();
			client_mutex.unlock();
		}

		void tcp_client::error_event(client_error_type error)
		{
			push_event(error);
			status = client_status_idle;
		}

		void tcp_client::thread_function(void * unused)
		{
			socket_descriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(socket_descriptor == invalid_socket_value)
			{
				error_event(client_error_socket);
				return;
			}

			if(address_string.empty() == false)
			{
				ulong address = ::inet_addr(address_string.c_str());
				if(address == INADDR_NONE)
				{
					status = client_status_lookup;
					::hostent * host_entity = ::gethostbyname(address_string.c_str());
					if(host_entity == 0)
					{
						error_event(client_error_dns);
						return;
					}
					address_number = *reinterpret_cast<ulong *>(host_entity->h_addr);
				}
				else
				{
					address_number = static_cast<ulong>(address);
				}
			}

			sockaddr_in server_address;
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = static_cast<u_long>(address_number);
			server_address.sin_port = ::htons(static_cast<u_short>(address_port));

			status = client_status_connecting;
			int result = ::connect(socket_descriptor, reinterpret_cast<sockaddr *>(&server_address), static_cast<int>(sizeof(sockaddr_in)));

			if(result == invalid_connect_value)
			{
				error_event(client_error_connect);
				return;
			}

			status = client_status_connected;
			push_event(client_event_connect);

			char buffer[buffer_size];

			while(terminate_thread == false)
			{
				result = ::recv(socket_descriptor, buffer, buffer_size, 0);

				if(result == 0)
				{
					push_event(client_event_disconnect);
					break;
				}

				else if(result == invalid_receive_value)
				{
#ifdef NIL_WINDOWS
					if(::WSAGetLastError() != WSAEWOULDBLOCK)
					{
						error_event(client_error_receive);
						break;
					}
#endif
					error_event(client_error_receive);
					break;
				}

				else
				{
					std::string data;
					data.assign(buffer, static_cast<std::size_t>(result));
					push_event(client_event(data));
				}
			}
			close_socket(socket_descriptor);
			status = client_status_idle;
		}

#ifdef NIL_WITH_SSL

		ssl_tcp_socket::ssl_tcp_socket():
			status(client_status_idle)
		{
			launch_winsock();
			launch_ssl();
		}

		ssl_tcp_socket::~ssl_tcp_socket()
		{
			disconnect();
		}

		bool ssl_tcp_socket::connect(ulong address, ulong port)
		{
			disconnect();
			socket_descriptor = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(socket_descriptor == invalid_socket_value)
				throw nil::exception("Unable to create TCP socket with SSL support");

			sockaddr_in server_address;
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = static_cast<u_long>(address);
			server_address.sin_port = ::htons(static_cast<u_short>(port));

			status = client_status_connecting;
			int result = ::connect(socket_descriptor, reinterpret_cast<sockaddr *>(&server_address), static_cast<int>(sizeof(sockaddr_in)));

			if(result == invalid_connect_value)
				return false;

			ctx = ::SSL_CTX_new(::SSLv2_client_method());
			ssl = ::SSL_new(ctx);

			::SSL_set_fd(ssl, socket_descriptor);

			result = ::SSL_connect(ssl);
			if(result != 1)
				return false;

			status = client_status_connected;
			return true;
		}

		bool ssl_tcp_socket::connect(std::string const & address, ulong port)
		{
			ulong result = ::inet_addr(address.c_str());
			ulong address_number;
			if(result == INADDR_NONE)
			{
				status = client_status_lookup;
				::hostent * host_entity = ::gethostbyname(address.c_str());
				if(host_entity == 0)
					return false;
				address_number = *reinterpret_cast<ulong *>(host_entity->h_addr);
			}
			else
				address_number = result;
			return connect(address_number, port);
		}

		void ssl_tcp_socket::disconnect()
		{
			if(status != client_status_idle && status != client_status_lookup)
			{
				::SSL_shutdown(ssl);
				::SSL_free(ssl);
				::SSL_CTX_free(ctx);
				close_socket(socket_descriptor);
				status = client_status_idle;
			}
		}

		bool ssl_tcp_socket::send(std::string const & input)
		{
			return send(input.c_str(), input.length());
		}

		bool ssl_tcp_socket::send(void const * input, std::size_t input_length)
		{
			if(status == client_status_connected)
			{
				int result = ::SSL_write(ssl, input, static_cast<int>(input_length));
				if(result <= 0)
				{
					status = client_status_idle;
					return false;
				}
				return true;
			}
			return false;
		}

		bool ssl_tcp_socket::receive(std::string & output)
		{
			if(status != client_status_connected)
				return false;
			char buffer[buffer_size];
			int result = ::SSL_read(ssl, buffer, static_cast<int>(buffer_size));
			if(result <= 0)
			{
				status = client_status_idle;
				return false;
			}
			output.assign(buffer, static_cast<std::size_t>(result));
			return true;
		}

		client_status_type ssl_tcp_socket::get_status() const
		{
			return status;
		}

		bool ssl_tcp_socket::get_ip(ulong & output)
		{
			if(status != client_status_connected)
				return false;
			sockaddr_in socket_address;
			socket_address_size_type socket_address_size = static_cast<socket_address_size_type>(sizeof(socket_address));
			int result = ::getsockname(socket_descriptor, reinterpret_cast<sockaddr *>(&socket_address), &socket_address_size);
			if(result == invalid_socket_name_value)
				return false;
			output = static_cast<ulong>(extract_address(socket_address));
			return true;
		}

#endif

				tcp_server_event::tcp_server_event()
		{
		}

		tcp_server_event::tcp_server_event(tcp_server_event_type type):
			type(type)
		{
		}

		tcp_server_event::tcp_server_event(tcp_server_error_type error):
			type(tcp_server_event_error),
			error(error)
		{
		}

		tcp_server_event::tcp_server_event(ulong client_identifier, tcp_server_event_type type):
			client_identifier(client_identifier),
			type(type)
		{
		}

		tcp_server_event::tcp_server_event(ulong client_identifier, std::string data):
			client_identifier(client_identifier),
			type(tcp_server_event_receive),
			data(data)
		{
		}

		tcp_server_event::tcp_server_event(ulong client_identifier, tcp_server_error_type error):
			client_identifier(client_identifier),
			type(tcp_server_event_error),
			error(error)
		{
		}

		tcp_server_client::tcp_server_client():
			to_disconnect(false)
		{
		}

		tcp_server::tcp_server():
			running(false)
		{
#ifdef NIL_WINDOWS
			winsock_launcher.launch();
#endif
		}

		tcp_server::~tcp_server()
		{
			shutdown();
		}

		bool tcp_server::listen(ulong server_port)
		{
			shutdown();

			port = server_port;
			terminate_thread = false;

			server_mutex.lock();

			server_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(server_socket == invalid_socket_value)
			{
				error_event(tcp_server_error_socket);
				return false;
			}

			sockaddr_in server_address;
			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = INADDR_ANY;
			server_address.sin_port = ::htons(static_cast<u_short>(port));

			int result = ::bind(server_socket, reinterpret_cast<sockaddr *>(&server_address), static_cast<int>(sizeof(sockaddr_in)));
			if(result == invalid_bind_value)
				return false;

			server_thread.start(thread::function_type(*this, &tcp_server::thread_function));
			return true;
		}

		void tcp_server::shutdown()
		{
			if(running)
			{
				terminate_thread = true;
				server_thread.join();
				running = false;
			}
		}

		void tcp_server::send(ulong client_identifier, std::string const & data)
		{
			//stupid code
			try
			{
				check_identifier(client_identifier);
			}
			catch(nil::exception &)
			{
				return;
			}

			server_mutex.lock();

			tcp_server_client & client = *client_map[client_identifier];

			int result = ::send(client.client_socket, data.c_str(), static_cast<int>(data.length()), 0);

			server_mutex.unlock();

			if(result == invalid_send_value)
				throw nil::exception("Failed to send data");
		}

		void tcp_server::disconnect(ulong client_identifier)
		{
			check_identifier(client_identifier);

			server_mutex.lock();
			tcp_server_client & client = *client_map[client_identifier];
			client.to_disconnect = true;
			server_mutex.unlock();
		}

		std::string tcp_server::get_ip(ulong client_identifier)
		{
			check_identifier(client_identifier);

			server_mutex.lock();
			tcp_server_client & client = *client_map[client_identifier];
			std::string output = ip_to_string(client.ip);
			server_mutex.unlock();
			return output;
		}

		tcp_server_event tcp_server::get_event()
		{
			server_mutex.lock();
			if(event_queue.empty())
			{
				server_mutex.unlock();
				queue_event.wait();
				server_mutex.lock();
			}
			tcp_server_event output = event_queue.front();
			server_mutex.unlock();
			return output;

			/*
			queue_event.wait();
			server_mutex.lock();
			tcp_server_event output = event_queue[0];
			event_queue.erase(event_queue.begin());
			if(event_queue.empty())
				queue_event.reset();
			cleanup();
			server_mutex.unlock();
			return output;
			*/
		}

		void tcp_server::check_identifier(ulong identifier)
		{
			if(!is_valid_identifier(identifier))
				throw nil::exception("Invalid client identifier");
		}

		bool tcp_server::is_valid_identifier(ulong identifier)
		{
			return client_map.find(identifier) != client_map.end();
		}

		void tcp_server::cleanup()
		{
			dead_client_type remaining_dead_clients;
			for(dead_client_type::iterator i = dead_clients.begin(), end = dead_clients.end(); i != end; i++)
			{
				ulong identifier = *i;
				bool hit = false;
				for(queue_type::iterator i = event_queue.begin(), end = event_queue.end(); i != end; i++)
				{
					if(i->client_identifier == identifier)
					{
						hit = true;
						break;
					}
				}
				if(hit)
					remaining_dead_clients.push_back(identifier);
				else
				{
					map_type::iterator dead_client = client_map.find(identifier);
					tcp_server_client * client = dead_client->second;
					client_map.erase(dead_client);
					delete client;
				}
			}
			dead_clients = remaining_dead_clients;
		}

		void tcp_server::error_event(tcp_server_error_type error)
		{
			event_queue.push_back(tcp_server_event(error));
			running = false;
			queue_event.set();
			server_mutex.unlock();
		}

		void tcp_server::push_event(tcp_server_event server_event)
		{
			event_queue.push_back(server_event);
			queue_event.set();
		}

		ulong tcp_server::get_unused_id()
		{
			for(ulong i = 0; i < ~0; i++)
				if(!is_valid_identifier(i))
					return i;
			throw nil::exception("Unable to get an unused client ID");
		}

		void tcp_server::thread_function(void * unused)
		{


			int result = ::listen(server_socket, SOMAXCONN);
			if(result == invalid_socket_value)
			{
				error_event(tcp_server_error_listen);
				return;
			}

			server_mutex.unlock();

			while(true)
			{
				sockaddr_in client_address;
				socket_address_size_type address_size = static_cast<socket_address_size_type>(sizeof(sockaddr_in));
				socket_type client_socket = ::accept(server_socket, reinterpret_cast<sockaddr *>(&client_address), &address_size);

				server_mutex.lock();

				if(terminate_thread)
				{
					server_mutex.unlock();
					break;
				}

				if(client_socket == invalid_socket_value)
				{
					error_event(tcp_server_error_accept);
					return;
				}

#ifdef NIL_WINDOWS
				ulong ip = client_address.sin_addr.S_un.S_addr;
#else
				ulong ip = client_address.sin_addr.s_addr;
#endif

				tcp_server_client * client_pointer = new tcp_server_client();
				tcp_server_client & client_object = *client_pointer;

				ulong client_id = get_unused_id();
				client_map[client_id] = client_pointer;

				client_object.identifier = client_id;
				client_object.client_socket = client_socket;
				client_object.ip = ip;
				client_object.client_thread.start(thread::function_type(*this, &tcp_server::client_thread), client_pointer);

				push_event(tcp_server_event(client_id, tcp_server_event_connect));

				server_mutex.unlock();
			}
		}

		void tcp_server::client_thread(void * client_void_pointer)
		{
			tcp_server_client * client_pointer = reinterpret_cast<tcp_server_client *>(client_void_pointer);
			tcp_server_client & client = *client_pointer;
			std::size_t const buffer_size = 2048;
			char buffer[buffer_size];
			while(true)
			{
				int bytes_read = ::recv(client.client_socket, buffer, static_cast<int>(buffer_size), 0);

				server_mutex.lock();

				if(client.to_disconnect)
				{
					close_socket(client.client_socket);
					dead_clients.push_back(client.identifier);
					break;
				}

				if(bytes_read == 0)
				{
					dead_clients.push_back(client.identifier);
					push_event(tcp_server_event(client.identifier, tcp_server_event_disconnect));
					break;
				}
				else if(bytes_read == invalid_socket_value)
				{
					dead_clients.push_back(client.identifier);
					push_event(tcp_server_event(client.identifier, tcp_server_error_receive));
					break;
				}

				std::string data(buffer, bytes_read);
				push_event(tcp_server_event(client.identifier, data));

				server_mutex.unlock();
			}

			server_mutex.unlock();
		}

		udp_client::udp_client():
			target_is_set(false)
		{
			launch_winsock();
			socket_descriptor = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if(socket_descriptor == invalid_socket_value)
				throw exception("Failed to create UDP socket");
			address.sin_family = AF_INET;
		}

		udp_client::~udp_client()
		{
			close_socket(socket_descriptor);
		}

		void udp_client::set_target(ulong target_address, ulong target_port)
		{
			target_is_set = true;
			address.sin_addr.s_addr = static_cast<u_long>(target_address);
			address.sin_port = ::htons(static_cast<u_short>(target_port));
		}

		bool udp_client::set_target(std::string const & target_address, ulong target_port)
		{
			target_is_set = true;
			char const * cstring_pointer = target_address.c_str();
			ulong address_long = ::inet_addr(cstring_pointer);
			if(address_long == INADDR_NONE)
			{
				::hostent * host_entity = ::gethostbyname(cstring_pointer);
				if(host_entity == 0)
					return false;
				address.sin_addr.s_addr = *host_entity->h_addr;
			}
			else
				address.sin_addr.s_addr = address_long;
			address.sin_port = ::htons(static_cast<u_short>(target_port));
			return true;
		}

		void udp_client::send(std::string const & data)
		{
			if(!target_is_set)
				throw exception("Can't send datagrams without a target");
			int result = ::sendto(socket_descriptor, data.c_str(), static_cast<int>(data.length()), 0, reinterpret_cast<sockaddr *>(&address), static_cast<int>(sizeof(sockaddr_in)));
			if(result == invalid_send_value)
				throw exception("Failed to send datagram");
		}

		std::string udp_client::receive()
		{
			if(!target_is_set)
				throw exception("Can't receive datagrams without a target");
			char buffer[1024];
			socket_address_size_type struct_size = static_cast<socket_address_size_type>(sizeof(address));
			int result = ::recvfrom(socket_descriptor, buffer, static_cast<int>(sizeof(buffer)), 0, reinterpret_cast<sockaddr *>(&address), &struct_size);
			if(result == invalid_receive_value)
				throw exception("Failed to receive datagram");
			return std::string(buffer, static_cast<std::size_t>(result));
		}

		udp_server::udp_server():
			is_running(false)
		{

		}

		udp_server::~udp_server()
		{
			shut_down();
		}

		bool udp_server::launch(ulong port)
		{
			shut_down();
			socket_descriptor = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
			if(socket_descriptor == invalid_socket_value)
				throw exception("Failed to create UDP server socket");
			sockaddr_in socket_address;
			socket_address.sin_family = AF_INET;
			socket_address.sin_port = ::htons(static_cast<u_short>(port));
			extract_address(socket_address) = INADDR_ANY;
			std::memset(socket_address.sin_zero, 0, sizeof(socket_address.sin_zero));

			socket_address_size_type socket_address_size = static_cast<socket_address_size_type>(sizeof(sockaddr_in));

			if(::bind(socket_descriptor, reinterpret_cast<sockaddr *>(&socket_address), socket_address_size) == invalid_bind_value)
				return false;

			is_running = true;
			return true;
		}

		void udp_server::shut_down()
		{
			if(is_running)
			{
				close_socket(socket_descriptor);
				is_running = false;
			}
		}

		udp_server_event udp_server::receive()
		{
			if(!is_running)
				throw nil::exception("Can't receive datagrams while the UDP server is shut down");
			char buffer[buffer_size];
			sockaddr_in client_address;
			socket_address_size_type socket_address_size = static_cast<socket_address_size_type>(sizeof(sockaddr_in));
			int result = ::recvfrom(socket_descriptor, buffer, static_cast<int>(buffer_size), 0, reinterpret_cast<sockaddr *>(&client_address), &socket_address_size);
			if(result == invalid_receive_value)
				throw nil::exception("Failed to read datagram from UDP server client");
			std::string data;
			data.assign(buffer, static_cast<std::size_t>(result));
			ulong ip = extract_address(client_address);
			udp_server_event output;
			output.data = data;
			output.ip = ip;
			return output;
		}

		ulong get_ip()
		{
			launch_winsock();
			char name_buffer[256];
			if(::gethostname(name_buffer, sizeof(name_buffer)) != 0)
				throw exception("Failed to retrieve host name");
			hostent * host_entity = ::gethostbyname(name_buffer);
			if(host_entity == 0)
				throw exception("Failed to retrieve host information");
			std::vector<ulong> addresses;
			for(std::size_t i = 0;; i++)
			{
				in_addr * address_pointer = reinterpret_cast<in_addr *>(host_entity->h_addr_list[i]);
				if(address_pointer == 0)
					break;
				addresses.push_back(static_cast<ulong>(address_pointer->s_addr));
			}
			return *(addresses.end() - 1);
		}

		std::string ip_to_string(ulong address)
		{
			address = static_cast<ulong>(::htonl(static_cast<long>(address)));
			std::stringstream stream;
			for(int i = 24; i > 0; i -= 8)
				stream << ((address >> i) & 0xff) << '.';
			stream << (address & 0xff);
			return stream.str();
		}
	}
}
