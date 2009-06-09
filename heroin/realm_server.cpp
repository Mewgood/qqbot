#include <heroin/client.hpp>

#include <iostream>

#include <nil/array.hpp>
#include <nil/random.hpp>
#include <nil/time.hpp>
#include <nil/string.hpp>

#include <heroin/utility.hpp>
#include <heroin/check_revision.hpp>
#include <heroin/bsha1.hpp>
#include <heroin/d2cdkey.hpp>
#include <heroin/compression.hpp>

bool get_mcp_packet(nil::net::tcp_socket & mcp_socket, std::string & buffer, std::string & data)
{
	std::string packet;
	while(buffer.size() < 3)
	{
		if (!mcp_socket.receive(packet)) {
			return false;
		}
		buffer += packet;
	}

	ulong packet_size = read_word(buffer, 0);
	while(packet_size > buffer.size())
	{
		if (!mcp_socket.receive(packet)) {
			return false;
		}
		buffer += packet;
	}

	data = buffer.substr(0, packet_size);
	buffer.erase(0, packet_size);
	return true;
}

void d2_client::mcp_thread_function(void * unused)
{

	std::cout << "[MCP]  " << "Connecting to realm server " << mcp_ip << ":" << mcp_port << std::endl;

	if (mcp_socket.connect(mcp_ip, mcp_port)) {
		if (debugging) std::cout << "[MCP]  " << "Successfully connected to realm server" << std::endl;
	} else
		std::cout << "[MCP]  " << "Failed to connect to the realm server" << std::endl;

	mcp_socket.send("\x01");

	std::string packet = construct_mcp_packet(0x01, mcp_data);
	mcp_socket.send(packet);


	std::string mcp_buffer;
	while (true) {
		std::string data;
		if (!get_mcp_packet(mcp_socket, mcp_buffer, data))
			break;
		ulong identifier = read_byte(data, 2);

		//std::cout << "[MCP]  " << "MCP data:" << std::endl;
		//std::cout << "[MCP]  " << data << std::endl;
		//print_data(data);

		if (identifier == 0x01) {
			ulong result = read_dword(data, 3);
			switch(result) {
				case 0x00:
					if (debugging) std::cout << "[MCP]  " << "Successfully logged on to realm server" << std::endl;
					break;

				case 0x7e:
					std::cout << "[MCP]  " << "Your CD key has been banned from playing on this realm!" << std::endl;
					break;

				case 0x7f:
					std::cout << "[MCP]  " << "Your IP has been banned temporarily!" << std::endl;
					break;

				default:
					std::cout << "[MCP]  " << "Unknown realm server logon error occured (" << std::hex << result << ")" << std::endl;
					break;
			}

			if (result != 0)
				return;

			// D2 only sends this the first time it connects, so...
			if (!logged_in) {
				if (debugging) std::cout << "[MCP]  " << "Requesting character list" << std::endl;
				mcp_socket.send(construct_mcp_packet(0x19, dword_to_string(8)));
			} else
				mcp_socket.send(construct_mcp_packet(0x07, character + zero));

		} else if (identifier == 0x19) {
			ulong count = read_word(data, 9);
			if (count == 0) {
				std::cout << "[MCP]  " << "There are no characters on this account." << std::endl;
				return;
			} else {
				bool found_character = false;
				bool selected_first_character = false;
				if (debugging) std::cout << "[MCP]  " << "List of characters on this account:" << std::endl;
				std::size_t offset = 11;
				for (ulong i = 1; i <= count; i++) {
					offset += 4;
					std::string character_name = read_string(data, offset);
					std::string stats = read_string(data, offset);
					ulong character_class = read_byte(stats, 13);
					ulong level = read_byte(stats, 25);
					ulong flags = read_byte(stats, 26);
					bool hardcore = (flags & 0x04) != 0;
					bool dead = (flags & 0x08) != 0;
					bool expansion = (flags & 0x20) != 0;
					std::string character_class_string = get_character_class_string(character_class);
					std::string core_string = hardcore ? "Hardcore" : "Softcore";
					std::string expansion_string = expansion ? "Expansion" : "Classic";
					if (debugging) std::cout << "[MCP]  " << i << ". " << character_name << ", Level " << level << " " << character_class_string << " (" << expansion_string << ", " << core_string;
					if (hardcore && dead)
						if (debugging) std::cout << ", Dead";
					if (debugging) std::cout << ")" << std::endl;
					//print_data(stats);
					if (character.empty() && i == 1) {
						selected_first_character = true;
						character = character_name;
					}

					if (character == character_name) {
						found_character = true;
						class_byte = character_class - 1;
						character_level = level;
					}
				}
				if (selected_first_character)
					std::cout << "[MCP]  " << "No character was specified in the ini file so the first character is being used" << std::endl;
				if (!found_character) {
					std::cout << "[MCP]  " << "Unable to locate character specified in the ini file";
					return;
				}

				std::cout << "[MCP]  " << "Logging into character " << character << std::endl;

				mcp_socket.send(construct_mcp_packet(0x07, character + zero));
			}
		} else if(identifier == 0x07) {
			ulong result = read_dword(data, 3);
			if (result != 0) {
				std::cout << "[MCP]  " << "Failed to log into character (" << std::hex << result << ")" << std::endl;
				return;
			}
			if (debugging) std::cout << "[MCP]  " << "Successfully logged into character" << std::endl;

			if (debugging) std::cout << "[MCP]  " << "Requesting channel list" << std::endl;
			bncs_socket.send(construct_bncs_packet(0x0b, lod_id));

			if (debugging) std::cout << "[MCP]  " << "Entering the chat server" << std::endl;
			bncs_socket.send(construct_bncs_packet(0x0a, character + zero + realm + "," + character + zero));

			if (!logged_in) {
				if (debugging) std::cout << "[MCP]  " << "Requesting MOTD" << std::endl;
				mcp_socket.send(construct_mcp_packet(0x12, ""));
				logged_in = true;
			}

		} else if (identifier == 0x12) {
			if (debugging) std::cout << "[MCP]  " << "Received MOTD" << std::endl;
		} else if (identifier == 0x03) {
			//print_data(data);

			ulong result = read_dword(data, 9);
			switch (result) {
				case 0x00:
					if (debugging) std::cout << "[MCP]  " << "Game has been created successfully" << std::endl;
					break;

				case 0x1e:
					std::cout << "[MCP]  " << "Invalid game name specified" << std::endl;
					break;

				case 0x1f:
					std::cout << "[MCP]  " << "This game already exists" << std::endl;
					break;

				case 0x20:
					std::cout << "[MCP]  " << "Game server down (it is probable that you tried to create a nightmare/hell game with a character who doesn't have access to that difficulty yet, or gamename/password were invalid)" << std::endl;
					break;

				case 0x6e:
					std::cout << "[MCP]  " << "Your character can't create a game because it's dead!" << std::endl;
					break;
			}

			if (result == 0) {
				if (debugging) std::cout << "[MCP]  " << "Joining the game we just created" << std::endl;
				join_game(game_name, game_password);
			}
		} else if (identifier == 0x04) {
			//print_data(data);

			ulong result = read_word(data, 17);

			switch(result) {
				case 0x00:
					if (debugging) std::cout << "[MCP]  " << "Successfully joined the game" << std::endl;
					break;

				case 0x29:
					std::cout << "[MCP]  " << "Password is incorrect" << std::endl;
					break;

				case 0x2A:
					std::cout << "[MCP]  " << "Game does not exist" << std::endl;
					break;

				case 0x2B:
					std::cout << "[MCP]  " << "Game is full" << std::endl;
					break;

				case 0x2C:
					std::cout << "[MCP]  " << "You do not meet the level requirements for this game" << std::endl;
					break;

				case 0x71:
					std::cout << "[MCP]  " << "A non-hardcore character cannot join a game created by a Hardcore character." << std::endl;
					break;

				case 0x73:
					std::cout << "[MCP]  " << "Unable to join a Nightmare game." << std::endl;
					break;

				case 0x74:
					std::cout << "[MCP]  " << "Unable to join a Hell game." << std::endl;
					break;

				case 0x78:
					std::cout << "[MCP]  " << "A non-expansion character cannot join a game created by an Expansion character." << std::endl;
					break;

				case 0x79:
					std::cout << "[MCP]  " << "A Expansion character cannot join a game created by a non-expansion character." << std::endl;
					break;

				case 0x7D:
					std::cout << "[MCP]  " << "A non-ladder character cannot join a game created by a Ladder character." << std::endl;
					break;
			}

			if (result == 0 || result == 0x2B) {
				ulong ip = read_dword(data, 9);
				gs_ip = nil::net::ip_to_string(ip);
				gs_hash = data.substr(13, 4);
				gs_token = data.substr(5, 2);

				bncs_socket.send(construct_bncs_packet(0x22, lod_id + dword_to_string(0xc) + game_name + zero + game_password + zero));
				bncs_socket.send(construct_bncs_packet(0x10, ""));

				gs_thread.start(nil::thread::function_type(*this, &d2_client::gs_thread_function));
				mcp_socket.disconnect();
			}
		}
	}
	std::cout << "[MCP]  " << "Disconnected from MCP server" << std::endl;
}
