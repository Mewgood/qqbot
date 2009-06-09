#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <map>
#include <vector>
#include <deque>

#include <nil/net.hpp>
#include <nil/thread.hpp>
#include <nil/ini.hpp>

#include <heroin/bit_reader.hpp>
#include <heroin/game.hpp>
#include <heroin/skills.hpp>
#include <heroin/item.hpp>
#include <heroin/container.hpp>
#include <heroin/types.hpp>
#include <heroin/data_manager.hpp>
#include <heroin/packetdef.hpp>

enum d2_client_status
{
    d2_client_status_idle,
    d2_client_status_active,
    d2_client_status_terminating
};

namespace character_skill_setup
{
    enum character_skill_setup_type
    {
        unknown_setup,
        sorceress_lightning,
        sorceress_meteor,
        sorceress_orb,
        sorceress_blizzard,
        paladin_hammerdin
    };
}

using character_skill_setup::character_skill_setup_type;

class d2_client
{
  public:

    d2_client(data_manager & client_data_manager);
    ~d2_client();

    void set_binary_information(std::string const & new_d2_binaries_directory, std::string const & new_game_exe_information);
    void set_realm_information(std::string const & new_battle_net_server, std::string const & new_realm);
    void set_key_data(std::string const & new_classic_key, std::string const & new_lod_key, std::string const & new_key_owner);
    void set_login_data(std::string const & new_account, std::string const & new_password, std::string const & new_character);
    void set_difficulty(std::string const & new_difficulty);
    void set_game_data(bool new_create_game, std::string const & new_game_name, std::string const & new_game_password);
    void set_inventory_settings(std::vector<std::string> const & lines);
    void set_chicken_life(ulong new_chicken_life);

    void launch();
    void terminate();
    void leave_game();

    void move_to(coordinate const & target);
    int lua_moveto(int x, int y);
    bool town_portal(void);
    void pickit(void);
    void weapon_swap(void);
    bool switch_skill(skill_type skill);
    bool switch_and_cast_on_self(skill_type skill);
    void cast_on_self();
    void cast_on_object(unsigned id);
    ulong left_skill;
    ulong right_skill;

    //debugging
    void npc_assignment(std::string const & data);
    ulong get_npc(std::string const name);
    ulong get_alive_npc(std::string const name, double range);
    ulong get_range_npc(double range);
    bool attack(ulong id);

    bool debugging;
    bool logged_in;
    nil::ini pick_list;
    player me;
    player_map players;
    std::map<unsigned, npc_type> npcs;
    std::map<unsigned, item_type> items;
    d2_client_status status;
    int games_this_session;

  protected:

    data_manager & client_data_manager;

    nil::thread bncs_thread, mcp_thread, gs_thread, ping_thread, bot_thread;
    nil::net::tcp_socket bncs_socket, mcp_socket, gs_socket;

    std::string
    d2_binaries_directory,
        game_exe_information,
        battle_net_server,
        realm,
        classic_key,
        lod_key,
        key_owner,
        account,
        password,
        character;

    bool create_game, connected_to_gs;
    std::string game_name, game_password;
    ulong server_token;

    std::string mcp_data, mcp_ip;
    ulong mcp_port;

    std::string gs_ip, gs_hash, gs_token;
    ulong class_byte, character_level;

    ulong game_request_id;

    game_difficulty_type difficulty;
    ulong last_timestamp;

    bool fully_entered_game;
    act_type current_act;
    ulong map_id, area_id;
    skill_map skill_levels, item_skill_levels;
    ulong experience;
    ulong current_life, chicken_life;

    bool on_the_move;
    coordinate last_reassignment, move_target;
    ulong last_teleport;

    container inventory, stash, cube, belt;
    std::vector<bool_line> inventory_settings;

    character_skill_setup_type character_skill_setup;


    //a few basic NPC IDs are stored here after detection
    ulong malah_id;

    bool talked_to_npc;
    bool entered_game;


    //virtual interface for your derived client - override these to process the events
    virtual void enter_game();
    virtual void receive_packet(std::string const & packet);
    virtual void experience_gained(ulong amount);
    virtual void player_moved(ulong player_id);
    virtual void chat_message(ulong player_id, std::string const & message);
    virtual void player_left(ulong player_id);
    virtual void player_joined(ulong player_id);
    virtual void new_npc(npc_type & npc);

    void send_packet(std::string const & packet);

    player & get_player(ulong id);
    player & get_player(std::string const & name);
    ulong get_skill_level(skill_type skill);

    void join_game(std::string const & join_game_name, std::string const & join_game_password);
    void request_reassignment();
    bool find_stash_space(item_type const & item, coordinate & output);
    bool talk_to_trader(ulong id);
    bool talk_to_healer(ulong id);

    void item_action(std::string const & data);

    std::string get_item_string(item_type const & item);
    std::string skill_levels_to_string();

  private:

    bool first_npc_info_packet;

    void bncs_thread_function(void * unused);
    void mcp_thread_function(void * unused);
    void gs_thread_function(void * unused);
    void ping_thread_function(void * unused);
    void process_game_packet(std::string const & packet);

    void initialise_game_data();
    void process_experience(ulong gained_experience);
    item_type parse_item(std::string const & data);
    void determine_character_skill_setup();
    void process_life_mana_packet(std::string const & data);
    void join_thread(nil::thread & thread, std::string const & description);

    void process_item_stat(unsigned stat_id, bit_reader & reader, item_property_type & item_property);
    ulong get_save_bits(unsigned stat_id);
    std::string get_skill_name(ulong skill);
};

extern std::string const
null,
    one,
    zero,
    platform,
    classic_id,
    lod_id;

ulong const bncs_port = 6112;
ulong const gs_port = 4000;

#endif
