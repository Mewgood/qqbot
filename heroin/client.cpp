#include <heroin/client.hpp>

#include <iostream>
#include <cmath>

#include <nil/string.hpp>
#include <nil/time.hpp>

#include <heroin/utility.hpp>
#include <heroin/item_data.hpp>
#include <heroin/bsha1.hpp>


extern std::string const
null("\x00\x00\x00\x00", 4),
    one("\x01\x00\x00\x00", 4),
    zero("\x00", 1),
    platform("68XI"),
    classic_id("VD2D"),
    lod_id("PX2D");

d2_client::d2_client(data_manager & client_data_manager):
    status(d2_client_status_idle),
    client_data_manager(client_data_manager),
    chicken_life(0),
    debugging(false)
{
}

d2_client::~d2_client()
{
    terminate();
}

void d2_client::set_binary_information(std::string const & new_d2_binaries_directory, std::string const & new_game_exe_information)
{
    d2_binaries_directory = new_d2_binaries_directory;
    game_exe_information = new_game_exe_information;

    fix_directory(d2_binaries_directory);
}

void d2_client::set_realm_information(std::string const & new_battle_net_server, std::string const & new_realm)
{
    battle_net_server = new_battle_net_server;
    realm = new_realm;
}

void d2_client::set_key_data(std::string const & new_classic_key, std::string const & new_lod_key, std::string const & new_key_owner)
{
    classic_key = new_classic_key;
    lod_key = new_lod_key;
    key_owner = new_key_owner;
}

void d2_client::set_login_data(std::string const & new_account, std::string const & new_password, std::string const & new_character)
{
    account = new_account;
    password = nil::string::to_lower(new_password);
    character = new_character;
}

void d2_client::set_game_data(bool new_create_game, std::string const & new_game_name, std::string const & new_game_password)
{
    create_game = new_create_game;
    game_name = new_game_name;
    game_password = new_game_password;
}

void d2_client::set_inventory_settings(std::vector<std::string> const & lines)
{
    std::vector<bool_line> new_inventory_settings;
    if(lines.size() != inventory_height)
        throw nil::exception("Invalid inventory height in inventory settings");
    for(std::size_t i = 0; i < inventory_height; i++)
    {
        std::string const & line = lines[i];
        if(line.size() != inventory_width)
            throw nil::exception("Invalid inventory width on at least one line of the inventory settings");
        bool_line new_line;
        for(std::size_t j = 0; j < inventory_width; j++)
        {
            char letter = line[j];
            switch(letter)
            {
            case '0':
                new_line.push_back(false);
                break;

            case '1':
                new_line.push_back(true);
                break;

            default:
                throw nil::exception("Encountered invalid byte in inventory settings string");
            }
        }
        new_inventory_settings.push_back(new_line);
    }
    inventory_settings = new_inventory_settings;
}

void d2_client::set_difficulty(std::string const & new_difficulty)
{
    std::string difficulty_string = nil::string::to_lower(new_difficulty);
    if(difficulty_string == "hell")
        difficulty = game_difficulty::hell;
    else if(difficulty_string == "nightmare")
        difficulty = game_difficulty::nightmare;
    else if(difficulty_string == "normal")
        difficulty = game_difficulty::normal;
    else
    {
        std::cout << "Unable to parse difficulty \"" << new_difficulty << "\", defaulting to normal" << std::endl;
        difficulty = game_difficulty::normal;
    }
}

void d2_client::set_chicken_life(ulong new_chicken_life)
{
    chicken_life = new_chicken_life;
}

void d2_client::launch()
{
    terminate();
    status = d2_client_status_active;
    bncs_thread.start(nil::thread::function_type(*this, &d2_client::bncs_thread_function));
}

void d2_client::terminate()
{
    if(status == d2_client_status_active)
    {
        if (debugging) std::cout << "Terminating client" << std::endl;

        status = d2_client_status_terminating;

        bncs_socket.disconnect();
        mcp_socket.disconnect();
        gs_socket.disconnect();

        //join_thread(bncs_thread, "BNCS");
        join_thread(mcp_thread, "MCP");
        join_thread(gs_thread, "GS");
        join_thread(ping_thread, "ping");

        if(debugging)
            std::cout << "Client has terminated" << std::endl;

        status = d2_client_status_idle;
    }
}

void d2_client::enter_game()
{
}

void d2_client::receive_packet(std::string const & packet)
{
}

void d2_client::experience_gained(ulong amount)
{
}

void d2_client::player_moved(ulong player_id)
{
}

void d2_client::chat_message(ulong player_id, std::string const & message)
{
}

void d2_client::player_left(ulong player_id)
{
}

void d2_client::player_joined(ulong player_id)
{
}

void d2_client::new_npc(npc_type & npc)
{
}

void d2_client::send_packet(std::string const & packet)
{
    gs_socket.send(packet);
}

void d2_client::initialise_game_data()
{
    fully_entered_game = false;

    last_teleport = 0;
    experience = 0;

    me = player();

    skill_levels.clear();
    item_skill_levels.clear();
    players.clear();
    npcs.clear();
    items.clear();

    inventory = container("Inventory", inventory_width, inventory_height);
    stash = container("Stash", stash_width, stash_height);
    cube = container("Cube", cube_width, cube_height);
    belt = container("Belt", 4, 4);

    malah_id = 0;
    current_life = 0;
    first_npc_info_packet = true;
}

void d2_client::process_experience(ulong gained_experience)
{
    bool first_time = (experience == 0);
    experience += gained_experience;
    if(!first_time)
        experience_gained(gained_experience);
}

player & d2_client::get_player(ulong id)
{
    if(id == me.id)
        return me;
    else
        return players[id];
}

player & d2_client::get_player(std::string const & name)
{
    for(player_map::iterator i = players.begin(); i != players.end(); i++)
    {
        player & current_player = i->second;
        if(current_player.name == name)
            return current_player;
    }
    return me;
}

ulong d2_client::get_skill_level(skill_type skill)
{
    return skill_levels[skill] + item_skill_levels[skill];
}

void d2_client::join_game(std::string const & join_game_name, std::string const & join_game_password)
{
    mcp_socket.send(construct_mcp_packet(0x04, word_to_string(game_request_id) + game_name + zero + game_password + zero));
    bncs_socket.send(construct_bncs_packet(0x1c, dword_to_string(1) + null + null + null + null + game_name + zero + game_password + zero + zero));
    game_request_id++;
}

void d2_client::request_reassignment()
{
    send_packet(std::string("\x4b\x00\x00\x00\x00", 5) + dword_to_string(me.id));
}

bool d2_client::find_stash_space(item_type const & item, coordinate & output)
{
    return stash.find_free_space(item, output);
}

bool d2_client::talk_to_trader(ulong id)
{
    talked_to_npc = false;
    npc_type & npc = npcs[id];
    double distance = me.location.distance(npc.location);

    if (debugging) std::cout << "[D2GS] " << "Attempting to talk to " << npc.name << std::endl;

    // Make the NPC move to us
    send_packet("\x59" + one + dword_to_string(id) + dword_to_string(me.location.x) + dword_to_string(me.location.y));
    long const sleep_step = 200;
    std::string move_to_entity = "\x04" + one + dword_to_string(id);
    //120 is more than necessary, should also be fixed for FRW
    for (long time_difference = static_cast<long>(distance * 120); time_difference > 0; time_difference -= sleep_step) {
        // the client sends the 'move to entity' packet every 200-300 ms or so, based on the distance to the NPC
        send_packet(move_to_entity);
        nil::sleep(std::min(sleep_step, time_difference));
    }
    // let's hope we're at the NPC now, talk to them
    send_packet("\x13" + one + dword_to_string(id));
    //print_data("\x13" + one + dword_to_string(id));
    nil::sleep(200);
    // Sent after 0x13, kinda useless but needed.
    send_packet("\x2f" + one + dword_to_string(id));
    //print_data("\x2f" + one + dword_to_string(id));
    // polling - what a fucking hack :( should use proper signaling...at least it's very portable
    long const timeout_step = 100;
    for (long npc_timeout = 4000; npc_timeout > 0 && !talked_to_npc && status == d2_client_status_active; npc_timeout -= timeout_step)
        nil::sleep(timeout_step);

    if(!talked_to_npc)
    {
        std::cout << "[D2GS] " << "Failed to talk to " << npc.name << "!" << std::endl;
        return false;
    }



    // Should now be at the dialog screen to "Chat, trade, gamble", etc.

    return true;
}

bool d2_client::talk_to_healer(ulong id)
{
    // unlike talk_to_trader this method immediately terminates the conversation with the NPC, used for healing only
    if(!talk_to_trader(id))
        return false;

    // terminate conversation
    send_packet("\x30" + one + dword_to_string(id));

    return true;
}

bool d2_client::attack(ulong id) {
    if (!connected_to_gs)
        return false;
    switch (character_skill_setup) {
    case character_skill_setup::sorceress_lightning:
        if (right_skill != skill::chain_lightning) {
            switch_skill(skill::chain_lightning);
        }
        cast_on_object(id);
        break;
    }

    return true;
}

void d2_client::leave_game() {
    entered_game = false;
    connected_to_gs = false;

    std::cout << "[D2GS] " << "Leaving the game" << std::endl;
    send_packet("\x69");

    nil::sleep(500);

    gs_socket.disconnect();

    bncs_socket.send(std::string("\xff\x1f\x04\x00", 4));
    nil::sleep(500);
    bncs_socket.send(construct_bncs_packet(0x40, ""));


    //bot_thread.terminate();
    join_thread(ping_thread, "ping");
    join_thread(gs_thread, "GS");
    join_thread(mcp_thread, "MCP");
}

void d2_client::move_to(coordinate const & target)
{
    ulong time = nil::time();
    if (time - last_teleport > 5) {
        send_packet("\x5f" + word_to_string(target.x) + word_to_string(target.y));
        last_teleport = time;
        nil::sleep(120);
    } else {
        //Manhattan distance
        //ulong distance = std::abs(target.x - me.x) + std::abs(target.y - me.y);
        //Euclidean distance
        double distance = me.location.distance(target);
        //std::cout << me.location.string() << std::endl;
        //std::cout << "Euclidean distance to move: " << distance << std::endl;
        send_packet("\x03" + word_to_string(target.x) + word_to_string(target.y));
        //requires FRW fix for better performance, this is for 0%
        nil::sleep(static_cast<ulong>(distance * 120));
    }

    me.location = target;
}

bool d2_client::town_portal(void)
{
    me.portal_id = 0;
    switch_skill(skill::book_of_townportal);
    cast_on_self();
    std::cout << "[D2GS] " << "Returning to town through TP." << std::endl;

    // 2 seconds should be long enough
    long const timeout_step = 100;
    for (long npc_timeout = 2000; npc_timeout > 0 && !me.portal_id; npc_timeout -= timeout_step)
        nil::sleep(timeout_step);

    if (!me.portal_id) {
        std::cout << "[D2GS] " << "Failed to take town portal!" << std::endl;
        return false;
    }

    send_packet(std::string("\x13\x02\x00\x00\x00", 5) + dword_to_string(me.portal_id));
    nil::sleep(300);

    return true;
}

void d2_client::weapon_swap(void)
{
    send_packet("\x60");
    nil::sleep(100);
}

bool d2_client::switch_skill(skill_type skill)
{
    if (skill_levels[skill] == 0) {
        return false;
    }
    right_skill = skill;
    send_packet("\x3c" + dword_to_string(skill) + "\xff\xff\xff\xff");
    nil::sleep(100);
    return true;
}

bool d2_client::switch_and_cast_on_self(skill_type skill) {
    if (switch_skill(skill)) {
        cast_on_self();
        return true;
    }
    return false;
}

void d2_client::cast_on_self()
{
    send_packet("\x0c" + word_to_string(me.location.x) + word_to_string(me.location.y));
    nil::sleep(300);
}

void d2_client::cast_on_object(unsigned id)
{
    send_packet(std::string("\x0d\x01\x00\x00\x00", 5) + dword_to_string(id));
    nil::sleep(300);
}

void d2_client::determine_character_skill_setup()
{
    if (debugging) std::cout << "Attempting to detect character skill setup automatically" << std::endl;
    //std::cout << me.character_class << std::endl;
    if (me.character_class == character_class::sorceress) {
        if (skill_levels[skill::blizzard] >= 15 && 
            skill_levels[skill::glacial_spike] >= 8 && 
            skill_levels[skill::ice_blast] >= 8) {
            std::cout << "[D2GS] " << "Using Blizzard/Glacial Spike/Ice Blast Sorceress setup." << std::endl;
            character_skill_setup = character_skill_setup::sorceress_orb;
        } else if (skill_levels[skill::meteor] >= 15 && 
                   skill_levels[skill::fire_ball] >= 15) {
            std::cout << "[D2GS] " << "Using Meteor/Fireball Sorceress setup." << std::endl;
            character_skill_setup = character_skill_setup::sorceress_meteor;
        } else if (skill_levels[skill::lightning] >= 15 && 
                   skill_levels[skill::chain_lightning]) {
            std::cout << "[D2GS] " << "Using Lightning/Chain Lightning Sorceress setup" << std::endl;
            character_skill_setup = character_skill_setup::sorceress_lightning;
        } else {
            std::cout << "Unknown Sorceress skill setup" << std::endl;
            character_skill_setup = character_skill_setup::unknown_setup;
        }
    } else if (me.character_class == character_class::paladin) {
        if (skill_levels[skill::blessed_hammer] >= 15 && 
            skill_levels[skill::concentration] >= 15) {
            std::cout << "[D2GS] " << "Using Hammerdin Paladin setup." << std::endl;
            character_skill_setup = character_skill_setup::paladin_hammerdin;
        } else {
            std::cout << "[D2GS] " << "Unknown Paladin skill setup." << std::endl;
            character_skill_setup = character_skill_setup::unknown_setup;
        }
    } else {
        std::cout << "No configuration available for this character class" << std::endl;
        character_skill_setup = character_skill_setup::unknown_setup;
    }
}

void d2_client::process_life_mana_packet(std::string const & data)
{
    // If this is the first time we've received 0x95, current_life won't be set
    // For some fucking retarded reason, D2GS sends 0x95 with your OLD (BO'd) life, and X/Y of 0x0000.
    if (read_word(data, 6) == 0x0000)
        return;

    unsigned plife = read_word(data, 1) & 0x7fff;

    if (current_life == 0)
        current_life = plife;

    if (plife < current_life && plife > 0) {
        unsigned damage = current_life - plife;
        std::cout << std::dec << damage << " damage is dealt to " << character << " (" << plife << " left)" << std::endl;
        if (plife <= 600) {
            std::cout << "Chickening with " << plife << " left!" << std::endl;
            leave_game();
        }
    }

    current_life = plife;
}

void d2_client::join_thread(nil::thread & thread, std::string const & description)
{
    if(debugging)
        std::cout << "Joining thread: " << description << std::endl;
    if(!thread.join() && debugging)
        std::cout << "Thread had already terminated: " << description << std::endl;
}

std::string d2_client::get_skill_name(ulong skill)
{
    std::string output;
    if(!client_data_manager.skills.get(static_cast<std::size_t>(skill), output))
        return "Unknown Skill";
    return output;
}

std::string d2_client::skill_levels_to_string() {
    std::ostringstream out;
    for ( skill_map::const_iterator iter = skill_levels.begin();
          iter != skill_levels.end();
          ++iter ) {
        if (iter->second == 0) {
            continue;
        }
        out << iter->first << " " << iter->second << "\n";
    }
    return out.str();
}
