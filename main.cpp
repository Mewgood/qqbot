#include <iostream>

#include <nil/ini.hpp>

#include <heroin/client.hpp>
#include <heroin/utility.hpp>

//to use the D2 client you derive a class from it
class qqbot: public d2_client
{
  public:
    qqbot(data_manager & manager):
        d2_client(manager)
    {
    }

  protected:
    void bot(void * unused);
    entity red_portal, harrogath_wp;
    npc_type pindle;
    char const * data_pointer;

    void receive_packet(std::string const & packet)
    {
        switch (get_byte(packet, 0)) {
        case D2GS_WORLDOBJECT: // Assigns objects (shrines, portals, torches, stash, chests...)
            data_pointer = packet.c_str();
            if (get_byte(packet, 1) == 0x02) {
                unsigned object = nil::read_little_endian(data_pointer + 6, 2);
                // Pindles portal
                if (object == 0x003c) {
                    red_portal.id = nil::read_little_endian(data_pointer + 2, 4);
                    red_portal.location.x = nil::read_little_endian(data_pointer + 8, 2);
                    red_portal.location.y = nil::read_little_endian(data_pointer + 10, 2);

                    if (debugging) std::cout << "[D2GS] " << "Received red portal ID and coordinates" << std::endl;
                }
                // A5 WP
                if (object == 0x01ad) {
                    harrogath_wp.id = nil::read_little_endian(data_pointer + 2, 4);
                    harrogath_wp.location.x = nil::read_little_endian(data_pointer + 8, 2);
                    harrogath_wp.location.y = nil::read_little_endian(data_pointer + 10, 2);

                    if (debugging) std::cout << "[D2GS] " << "Received A5 WP id and coordinates" << std::endl;
                }

            }
            break;

        case D2GS_ASSIGNPLAYER: // Player has entered the game.
            if (!entered_game) {
                bot_thread.start(nil::thread::function_type(*this, &qqbot::bot));
                entered_game = true;
            }

            break;

            /*case D2GS_ASSIGNNPC:
            // game_server.cpp parses this now
            data_pointer = packet.c_str();
            unsigned id = nil::read_little_endian(data_pointer + 1, 4);
            unsigned npc_id = nil::read_little_endian(data_pointer + 5, 2);
            unsigned x = nil::read_little_endian(data_pointer + 7, 2);
            unsigned y = nil::read_little_endian(data_pointer + 9, 2);

            if (npc_id == 0x01b8) {

            if (packet.size() > 0x18) {
            std::cout << "[D2GS] " << "Found pindle!" << std::endl;
            pindle.location.x = x;
            pindle.location.y = y;
            pindle.id = id;
            }
            }

            break;*/
        }
    }

    void new_npc(npc_type & npc) {
        npcs[npc.id] = npc;
        //std::cout << "[D2GS] " << "Name: " << npcs[npc.id].name << ", ID: " << std::hex << npc.id << std::endl;
    }
};


void qqbot::bot(void * unused) {
    if (debugging) std::cout << "[D2GS] " << "Entered bot thread" << std::endl;

    nil::sleep(2000);
    std::cout << qqbot::skill_levels_to_string() << std::endl;
    std::cout << "[D2GS] " << "Starting Pindleskin sequence" << std::endl;

    // Move to Malah
    qqbot::move_to(coordinate::coordinate(5089, 5019));
    qqbot::move_to(coordinate::coordinate(5090, 5030));
    qqbot::move_to(coordinate::coordinate(5082, 5033));
    qqbot::move_to(coordinate::coordinate(5074, 5033));

    // Heal
    npc_type malah = npcs[get_npc("Malah")];
    qqbot::talk_to_trader(malah.id);
    nil::sleep(1000);
    // Open trade screen.
    send_packet("\x38" + one + dword_to_string(malah.id) + std::string("\x00\x00\x00\x00", 4));
    //print_data("\x38" + one + dword_to_string(malah.id) + std::string("\x00\x00\x00\x00", 4));
    nil::sleep(2500);
    for (std::map<unsigned, item_type>::iterator it = items.begin(); it != items.end(); ++it) {
        if (it->second.action == item_action::add_to_shop) {
            if (it->second.type == "tsc") {
                std::cout << "[D2GS] " << "Buying TP." << std::endl;
                send_packet("\x32" + dword_to_string(malah.id) + dword_to_string(it->second.id) + std::string("\x00\x00\x00\x00", 4) + std::string("\x02\x00\x00\x00", 4));
                //print_data("\x32" + dword_to_string(malah.id) + dword_to_string(it->second.id) + std::string("\x00\x00\x00\x00", 4) + std::string("\x02\x00\x00\x00", 4));
            }
        }
    }
    nil::sleep(1000);
    // End the trade.
    send_packet("\x30" + one + dword_to_string(malah.id));
    //print_data("\x30" + one + dword_to_string(malah.id));
    nil::sleep(500);

    // Move to portal
    qqbot::move_to(coordinate::coordinate(5073, 5032));
    qqbot::move_to(coordinate::coordinate(5073, 5044));
    qqbot::move_to(coordinate::coordinate(5078, 5055));
    qqbot::move_to(coordinate::coordinate(5081, 5065));
    qqbot::move_to(coordinate::coordinate(5081, 5076));
    qqbot::move_to(coordinate::coordinate(5082, 5087));
    qqbot::move_to(coordinate::coordinate(5085, 5098));
    qqbot::move_to(coordinate::coordinate(5088, 5110));
    qqbot::move_to(coordinate::coordinate(5093, 5121));
    qqbot::move_to(coordinate::coordinate(5103, 5124));
    qqbot::move_to(coordinate::coordinate(5111, 5121));

    nil::sleep(500);
    // Take the portal
    qqbot::send_packet(std::string("\x13\x02\x00\x00\x00", 5) + dword_to_string(red_portal.id));

    nil::sleep(500);


    qqbot::weapon_swap();
    switch_and_cast_on_self(skill::battle_command);
    switch_and_cast_on_self(skill::battle_orders);
    nil::sleep(500);
    qqbot::weapon_swap();

    // Teleport
    switch_skill(skill::teleport);
    nil::sleep(300);
    send_packet("\x0c\x50\x27\xe6\x33");
    nil::sleep(300);
    send_packet("\x0c\x4d\x27\xcc\x33");
    nil::sleep(300);
    send_packet("\x0c\x4a\x27\xb7\x33");
    nil::sleep(300);

    switch_skill(skill::chain_lightning);

    ulong id = get_npc("Pindleskin");
    if (!id) {
        std::cout << "[D2GS] " << "Unable to find Pindle, probably got stuck." << std::endl;
        leave_game();
        return;
    }

    while (npcs[id].life > 0) {
        if (!qqbot::attack(id))
            return;
        std::cout << "[D2GS] " << npcs[id].name << "'s life is: " << std::dec << npcs[id].life << std::endl;
    }

    while ((id = get_alive_npc("Defiled Warrior", 20))) {
        while (npcs[id].life > 0) {
            if (!qqbot::attack(id))
                return;
            std::cout << "[D2GS] " << npcs[id].name << "'s life is: " << std::dec << npcs[id].life << std::endl;
        }
    }

    // Barely working pickit!
    for (std::map<unsigned, item_type>::iterator it = items.begin(); it != items.end(); ++it) {
        if ((pick_list.string(it->second.type + get_quality(it->second.quality), "pickit", "no") == "yes" && me.location.distance(coordinate::coordinate(it->second.x, it->second.y)) < 30)
            || (pick_list.string(it->second.type, "pickit", "no") == "yes" && me.location.distance(coordinate::coordinate(it->second.x, it->second.y)) < 30)) {
            std::cout << "[D2GS] " << "Attempting to pick up " << it->second.name << " (" << it->second.type << get_quality(it->second.quality) << ")" << std::endl;
            qqbot::move_to(coordinate::coordinate(it->second.x, it->second.y));
            send_packet(std::string("\x16\x04\x00\x00\x00", 5) + dword_to_string(it->second.id) + std::string("\x00\x00\x00\x00", 4));
            nil::sleep(1000);
        }
    }

    // TP
    if (!town_portal())
        return;

    // Move to way point
    qqbot::move_to(coordinate::coordinate(5104, 5045));
    qqbot::move_to(coordinate::coordinate(5114, 5069));

    // Take waypoint
    send_packet(std::string("\x13\x02\x00\x00\x00", 5) + dword_to_string(harrogath_wp.id));
    nil::sleep(500);
    send_packet("\x49" + dword_to_string(harrogath_wp.id) + std::string("\x6f\x00\x00\x00", 4));

    // Teleport to Eld
    switch_skill(skill::teleport);
    nil::sleep(200);
    send_packet("\x0c\xa0\x0e\xd8\x13"); // X: 3744; Y: 5080
    nil::sleep(200);


    // Kill them ^_^
    //switch_skill(0x35);
    id = get_npc("Eldritch the Rectifier");
    if (!id) {
        std::cout << "[D2GS] " << "Unable to find Eldritch, probably got stuck." << std::endl;
        leave_game();
        return;
    }
    while (npcs[id].life > 0) {
        if (!qqbot::attack(id))
            return;
        std::cout << "[D2GS] " << npcs[id].name << "'s life is: " << std::dec << npcs[id].life << std::endl;
    }

    while ((id = get_range_npc(25))) {
        while (npcs[id].life > 0) {
            if (!qqbot::attack(id))
                return;
            std::cout << "[D2GS] " << npcs[id].name << "'s life is: " << std::dec << npcs[id].life << std::endl;
        }
    }

    switch_skill(skill::teleport);
    nil::sleep(200);
    send_packet("\x0c\x96\x0e\xcc\x13"); // X: 3734; Y: 5068

    // Barely working pickit!
    for (std::map<unsigned, item_type>::iterator it = items.begin(); it != items.end(); ++it) {
        if ((pick_list.string(it->second.type + get_quality(it->second.quality), "pickit", "no") == "yes" && me.location.distance(coordinate::coordinate(it->second.x, it->second.y)) < 30)
            || (pick_list.string(it->second.type, "pickit", "no") == "yes" && me.location.distance(coordinate::coordinate(it->second.x, it->second.y)) < 30)) {
            std::cout << "[D2GS] " << "Attempting to pick up " << it->second.name << " (" << it->second.type << get_quality(it->second.quality) << ")" << std::endl;
            qqbot::move_to(coordinate::coordinate(it->second.x, it->second.y));
            send_packet(std::string("\x16\x04\x00\x00\x00", 5) + dword_to_string(it->second.id) + std::string("\x00\x00\x00\x00", 4));
            nil::sleep(1000);
        }
    }

    leave_game();

    std::cout << "[D2GS] " << "Bot thread terminating" << std::endl;
}


int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cout << "[D2GS] " << argv[0] << " <ini>" << std::endl;
        return 0;
    }

    nil::ini ini;
    if (!ini.load(argv[1])) {
        std::cout << "[D2GS] " << "Unable to load ini file from specified directory." << std::endl;
        return 1;
    }

    try {
        // Load the INI file
        std::string
            d2_binary_directory = ini.string("config", "data_directory"),
            text_data_directory = ini.string("config", "data_directory"),

            game_exe_information = ini.string("config", "game_exe_information"),
            battle_net_server = ini.string("config", "battle_net_server"),
            realm = ini.string("config", "realm", ""), //only relevant for Asia multi realm servers
            account = ini.string("config", "account"),
            password = ini.string("config", "password"),
            character = ini.string("config", "character", ""),
            difficulty = ini.string("config", "difficulty", "normal");

        int i = 0;
        std::string classic_keys[10];
        std::string lod_keys[10];
        std::stringstream ss;
        int keys;
        while (true) {
            ss.str("");
            ss << "key" << i;
            if (ini.string(ss.str(), "classic_key", "no") != "no") {
                classic_keys[i] = ini.string(ss.str(), "classic_key", "no");
                lod_keys[i] = ini.string(ss.str(), "lod_key", "no");
            } else {
                keys = i - 1;
                break;
            }
            i++;
        }

        //the data manager handles the heroin_glands data .txt files and the Diablo II .bin files
        //it is stored independently from the client objects because one manager can be used by all clients
        data_manager manager(text_data_directory);
        qqbot client(manager);

        client.pick_list.load(ini.string("config", "pickit"));
        client.debugging = true;
        client.logged_in = false;
        client.set_binary_information(d2_binary_directory, game_exe_information);
        client.set_realm_information(battle_net_server, realm);
        client.set_key_data(classic_keys[0], lod_keys[0], "qq.BOT");
        client.set_login_data(account, password, character);
        client.set_difficulty(difficulty);
        client.set_game_data(true, "", "");


        i = 0;
        while (true) {
            client.set_key_data(classic_keys[i], lod_keys[i], "qq.BOT");
            client.games_this_session = 0;
            std::cout << "[BOT]  CD-Key set to key " << i << "." << std::endl;
            client.launch();

            nil::sleep(5000);

            while (client.status != d2_client_status_idle)
                nil::sleep(1000);
            if (i == keys)
                break;
            else
                i++;
        }
    }
    catch(nil::exception & exception)
    {
        std::cout << "[D2GS] " << "nil exception: " << exception.get_message() << std::endl;
    }
    catch(std::exception & exception)
    {
        std::cout << "[D2GS] " << "std exception: " << exception.what() << std::endl;
    }

    return 0;
}
