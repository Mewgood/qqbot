#include <heroin/client.hpp>

#include <iostream>

#include <nil/array.hpp>
#include <nil/time.hpp>
#include <nil/string.hpp>
#include <nil/random.hpp>

#include <heroin/utility.hpp>
#include <heroin/check_revision.hpp>
#include <heroin/bsha1.hpp>
#include <heroin/d2cdkey.hpp>
#include <heroin/compression.hpp>

namespace
{
	unsigned char const auth_info_packet[] =
	{
		0xff, 0x50, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x36, 0x38, 0x58, 0x49, 0x50, 0x58, 0x32, 0x44,
		0x0c, 0x00, 0x00, 0x00, 0x53, 0x55, 0x6e, 0x65,
		0x55, 0xb4, 0x47, 0x40, 0x88, 0xff, 0xff, 0xff,
		0x09, 0x04, 0x00, 0x00, 0x09, 0x04, 0x00, 0x00,
		0x55, 0x53, 0x41, 0x00, 0x55, 0x6e, 0x69, 0x74,
		0x65, 0x64, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65,
		0x73, 0x00
	};
}

bool get_bncs_packet(nil::net::tcp_socket & bncs_socket, std::string & buffer, std::string & data)
{
	std::string packet;

	while(buffer.size() < 4)
	{
		if(!bncs_socket.receive(packet))
			return false;
		buffer += packet;
	}

	ulong packet_size = read_word(buffer, 2);
	while(packet_size > buffer.size())
	{
		if(!bncs_socket.receive(packet))
			return false;
		buffer += packet;
	}

	data = buffer.substr(0, packet_size);
	buffer.erase(0, packet_size);
	return true;
}

ulong convert_game_difficulty(game_difficulty_type difficulty)
{
	switch(difficulty)
	{
		default:
		case game_difficulty::normal:
			return 0x0000;

		case game_difficulty::nightmare:
			return 0x1000;

		case game_difficulty::hell:
			return 0x2000;
	}
}

void d2_client::bncs_thread_function(void * unused)
{
	game_request_id = 0x02;
	entered_game = false;

	std::cout << "[BNCS] " << "Connecting to " << battle_net_server << ":" << bncs_port << "..." << std::endl;
	if (bncs_socket.connect(battle_net_server, bncs_port)) {
		if (debugging) std::cout << "[BNCS] " << "Connected to BNCS" << std::endl;
	} else {
		std::cout << "[BNCS] " << "Failed to connect to BNCS" << std::endl;
		return;
	}

	bncs_socket.send("\x01");
	bncs_socket.send(auth_info_packet, nil::countof(auth_info_packet));

	std::string bncs_buffer;

	while(true)
	{
		std::string data;
		if(!get_bncs_packet(bncs_socket, bncs_buffer, data))
			break;

		//std::cout << "[BNCS] " << "BNCS data: " << data << std::endl;
		//print_data(data);

		ulong type = get_byte(data, 1);
		if(type == 0x00 || type == 0x25)
		{
			//ping
			//std::cout << "[BNCS] " << "Replying to ping" << std::endl;
			bncs_socket.send(data);
		}
		else if(type == 0x50)
		{
			server_token = read_dword(data, 8);
			std::string mpq_file_time = data.substr(16, 8);

			std::size_t offset = 24;
			std::string mpq_file_name = read_string(data, offset);
			std::string formula_string = read_string(data, offset);

			if (debugging) {
				std::cout << "[BNCS] " << "Received EXE/CD key hash request" << std::endl;
				std::cout << "[BNCS] " << "Server token: " << get_dword_string(server_token) << std::endl;
				std::cout << "[BNCS] " << "MPQ file time: " << get_data_string(mpq_file_time) << std::endl;
				std::cout << "[BNCS] " << "MPQ file name: " << mpq_file_name << std::endl;
				std::cout << "[BNCS] " << "Formula: " << formula_string << std::endl;
			}

			//imitate the Diablo II client and download the MPQ file in question via BNFTP

			nil::net::tcp_socket bnftp_socket;

			if(bnftp_socket.connect(battle_net_server, bncs_port)) {
				if (debugging) std::cout << "[BNCS] " << "Connected to BNCS for BNFTP transfer" << std::endl;
			} else {
				std::cout << "[BNCS] " << "Failed to connect to BNCS for BNFTP transfer" << std::endl;
				return;
			}

			bnftp_socket.send("\x02");
			bnftp_socket.send(std::string("\x2f\x00\x00\x01", 4) + platform + classic_id + null + null + null + mpq_file_time + mpq_file_name + zero);

			//read file size
			std::string bnftp_buffer;
			ulong bytes_received = 0;

			do
			{
				bnftp_socket.receive(data);
				bnftp_buffer += data;
				bytes_received += data.length();
			}
			while(bytes_received < 8);

			ulong header_size = read_word(bnftp_buffer, 0);
			ulong file_size = read_dword(bnftp_buffer, 4);
			ulong total_size = header_size + file_size;

			if (debugging) std::cout << "[BNCS] " << "Starting BNFTP download" << std::endl;

			//start BNFTP download
			do
			{
				if(!bnftp_socket.receive(data))
					break;
				bytes_received += data.length();
			}
			while(bytes_received < total_size);

			if (debugging) std::cout << "[BNCS] " << "Finished BNFTP download" << std::endl;

			bnftp_socket.disconnect();

			ulong exe_checksum;
			switch(check_revision(formula_string, mpq_file_name, d2_binaries_directory, exe_checksum))
			{
				case check_revision_result_success:
					if (debugging) std::cout << "[BNCS] " << "Successfully generated EXE checksum" << std::endl;
					break;

				case check_revision_result_formula_error:
					std::cout << "[BNCS] " << "Unable to parse formula string" << std::endl;
					return;

				case check_revision_result_mpq_error:
					std::cout << "[BNCS] " << "Unable to parse MPQ file name" << std::endl;
					return;

				case check_revision_result_file_error:
					std::cout << "[BNCS] " << "Unable to read D2 binaries - you probably specified the wrong path in the ini file or you are running Diablo II with them" << std::endl;
					return;

				default:
					std::cout << "[BNCS] " << "Unknown check revision result" << std::endl;
					return;
			}

			ulong client_token = get_tick_count();

			std::string classic_hash, lod_hash, classic_public, lod_public;

			if (hash_d2key(classic_key, client_token, server_token, classic_hash, classic_public)) {
				if (debugging) std::cout << "[BNCS] " << "Successfully generated the classic CD key hash" << std::endl;
			} else {
				std::cout << "[BNCS] " << "Your classic CD key is invalid!" << std::endl;
				return;
			}

			if (hash_d2key(lod_key, client_token, server_token, lod_hash, lod_public)) {
				if (debugging) std::cout << "[BNCS] " << "Successfully generated the expansion CD key hash" << std::endl;
			} else {
				std::cout << "[BNCS] " << "Your expansion CD key is invalid!" << std::endl;
				return;
			}

			bool hash_d2key(std::string const & cdkey, ulong client_token, ulong server_token, std::string & output);

			std::string arguments = dword_to_string(client_token) + dword_to_string(0x01000001) + dword_to_string(exe_checksum) + dword_to_string(2) + null + dword_to_string(0x10) + dword_to_string(6) + classic_public + null + classic_hash;
			arguments += dword_to_string(0x10) + dword_to_string(10) + lod_public + null + lod_hash + game_exe_information + zero + key_owner + zero;
			std::string packet = construct_bncs_packet(0x51, arguments);

			//print_data(packet);

			bncs_socket.send(packet);
		}
		else if(type == 0x51)
		{
			ulong result = read_dword(data, 4);
			std::string information = read_one_string(data, 8);
			switch(result)
			{
				case 0x000:
					if (debugging) std::cout << "[BNCS] " << "Successfully logged on to Battle.net" << std::endl;
					break;

				case 0x100:
					std::cout << "[BNCS] " << "Outdated game version" << std::endl;
					break;

				case 0x101:
					std::cout << "[BNCS] " << "Invalid version" << std::endl;
					break;

				case 0x102:
					std::cout << "[BNCS] " << "Game version must be downgraded to " << information << std::endl;
					break;

				case 0x200:
					std::cout << "[BNCS] " << "Invalid CD key" << std::endl;
					break;

				case 0x201:
					std::cout << "[BNCS] " << "CD key is being used by " << information << std::endl;
					break;

				case 0x202:
					std::cout << "[BNCS] " << "This CD key has been banned" << std::endl;
					break;

				case 0x203:
					std::cout << "[BNCS] " << "This CD key is meant to be used with another product" << std::endl;
					break;

				default:
					std::cout << "[BNCS] " << "Failed to log on to Battle.net (" << std::hex << result << ")" << std::endl;
					break;
			}
			if (result == 0) {
				if (debugging) std::cout << "[BNCS] " << "Requesting ini file time" << std::endl;
				std::string packet = construct_bncs_packet(0x33, dword_to_string(0x80000004) + null + "bnserver-D2DV.ini" + zero);
				//print_data(packet);
				bncs_socket.send(packet);
			} else
				return;
		}
		else if(type == 0x33)
		{
			if (debugging) std::cout << "[BNCS] " << "Logging into the account" << std::endl;
			ulong client_token = nil::time();
			std::string hash = double_hash(client_token, server_token, password);
			std::string packet = construct_bncs_packet(0x3a, dword_to_string(client_token) + dword_to_string(server_token) + hash + account + zero);
			//print_data(packet);
			bncs_socket.send(packet);
		}
		else if(type == 0x3a)
		{
			ulong result = read_dword(data, 4);
			switch(result)
			{
				case 0x00:
					std::cout << "[BNCS] " << "Successfully logged into the account " << account << std::endl;
					break;

				case 0x01:
					std::cout << "[BNCS] " << "Account does not exist" << std::endl;
					break;

				case 0x02:
					std::cout << "[BNCS] " << "Invalid password specified" << std::endl;
					break;

				case 0x06:
					std::cout << "[BNCS] " << "Account has been closed" << std::endl;
					break;

				default:
					std::cout << "[BNCS] " << "Unknown login error (" << std::hex << result << ")" << std::endl;
					break;
			}

			if(result == 0)
				bncs_socket.send(construct_bncs_packet(0x40, ""));
			else
				return;
		}
		else if(type == 0x40)
		{
			ulong count = read_dword(data, 8);
			std::size_t offset = 12;
			if (debugging) std::cout << "[BNCS] " << "List of realms:" << std::endl;
			for(ulong i = 1; i <= count; i++)
			{
				offset += 4;
				std::string realm_title = read_string(data, offset);
				std::string realm_description = read_string(data, offset);
				if (debugging) std::cout << "[BNCS] " << i << ". " << realm_title << ", " << realm_description << std::endl;

				if(realm.empty() && i == 1)
				{
					if (debugging) std::cout << "[BNCS] " << "No realm was specified in the ini so we're going to connect to " << realm_title << std::endl;
					realm = realm_title;
				}
			}

			std::cout << "[BNCS] " << "Logging on to the realm " << realm << std::endl;

			ulong client_token = 1;
			std::string packet = construct_bncs_packet(0x3e, dword_to_string(client_token) + double_hash(client_token, server_token, "password") + realm + zero);
			//print_data(packet);
			bncs_socket.send(packet);
		}
		else if(type == 0x3e)
		{
			if(data.size() <= 12)
			{
				std::cout << "[BNCS] " << "Failed to log on to realm:" << std::endl;
				//print_data(data);
				return;
			}

			//print_data(data);
			//cluster fuck
			//ulong ip = read_nbo_dword(data, 20);
			ulong ip = read_dword(data, 20);
			mcp_port = read_nbo_word(data, 24);
			mcp_ip = nil::net::ip_to_string(ip);
			std::size_t offset = 28;
			mcp_data = data.substr(4, 16) + data.substr(offset, data.size() - offset);

			mcp_thread.start(nil::thread::function_type(*this, &d2_client::mcp_thread_function));
		}
		else if(type == 0x0a)
		{
			std::cout << "[BNCS] " << "Entered the chat." << std::endl;
			if (debugging) std::cout << "[BNCS] " << "Requesting news and advertising data" << std::endl;
			bncs_socket.send(construct_bncs_packet(0x46, null));
			bncs_socket.send(construct_bncs_packet(0x15, platform + lod_id + null + dword_to_string(get_tick_count())));

			if (games_this_session >= 10) {
			    std::cout << "[BNCS] " << "Switching CD-Keys after running " << games_this_session << " games." << std::endl;
                terminate();
                return;
			} else
                games_this_session++;

			if (game_password.empty())
				//game_password = generate_string();
				game_password = "h4x";


			if (create_game) {
                game_name = generate_string();
				// If we don't sleep here, you get disconnected during the game for making games too fast.
				nil::sleep(10000);

				std::cout << "[MCP]  " << "Creating game \"" << game_name << "\" with password \"" << game_password << "\"" << std::endl;

				std::string arguments = word_to_string(game_request_id) + dword_to_string(convert_game_difficulty(difficulty)) + "\x01\x0ff\x08" + game_name + zero + game_password + zero + zero;
				mcp_socket.send(construct_mcp_packet(0x03, arguments));
				game_request_id++;
			} else {
				std::cout << "[MCP]  " << "Joining game \"" << game_name << "\" with password \"" << game_password << "\"" << std::endl;
				join_game(game_name, game_password);
			}

		}
		else if(type == 0x15)
		{
			ulong ad_id = read_dword(data, 4);
			if (debugging) std::cout << "[BNCS] " << "Received advertising data, sending back display confirmation" << std::endl;
			bncs_socket.send(construct_bncs_packet(0x21, platform + lod_id + dword_to_string(ad_id) + zero + zero));
		}
	}
	std::cout << "[BNCS] " << "Disconnected from BNCS" << std::endl;
}
