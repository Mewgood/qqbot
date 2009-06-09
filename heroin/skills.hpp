// Do not modify
// Generated from generateSkillsFiles.pl
// list stolen from D2BS SDK, run through a Python script to generate enums
#ifndef SKILLS_HPP
#define SKILLS_HPP
namespace skill
{
    enum skill_type 
    {
        attack,                        //    0 0x0000
        kick,                          //    1 0x0001
        throw_stuff,                   //    2 0x0002
        unsummon,                      //    3 0x0003
        left_hand_throw,               //    4 0x0004
        left_hand_swing,               //    5 0x0005
        magic_arrow,                   //    6 0x0006
        fire_arrow,                    //    7 0x0007
        inner_sight,                   //    8 0x0008
        critical_strike,               //    9 0x0009
        jab,                           //   10 0x000a
        cold_arrow,                    //   11 0x000b
        multiple_shot,                 //   12 0x000c
        dodge,                         //   13 0x000d
        power_strike,                  //   14 0x000e
        poison_javelin,                //   15 0x000f
        exploding_arrow,               //   16 0x0010
        slow_missiles,                 //   17 0x0011
        avoid,                         //   18 0x0012
        impale,                        //   19 0x0013
        lightning_bolt,                //   20 0x0014
        ice_arrow,                     //   21 0x0015
        guided_arrow,                  //   22 0x0016
        penetrate,                     //   23 0x0017
        charged_strike,                //   24 0x0018
        plague_javelin,                //   25 0x0019
        strafe,                        //   26 0x001a
        immolation_arrow,              //   27 0x001b
        dopplezon,                     //   28 0x001c
        evade,                         //   29 0x001d
        fend,                          //   30 0x001e
        freezing_arrow,                //   31 0x001f
        valkyrie,                      //   32 0x0020
        pierce,                        //   33 0x0021
        lightning_strike,              //   34 0x0022
        lightning_fury,                //   35 0x0023
        fire_bolt,                     //   36 0x0024
        warmth,                        //   37 0x0025
        charged_bolt,                  //   38 0x0026
        ice_bolt,                      //   39 0x0027
        frozen_armor,                  //   40 0x0028
        inferno,                       //   41 0x0029
        static_field,                  //   42 0x002a
        telekinesis,                   //   43 0x002b
        frost_nova,                    //   44 0x002c
        ice_blast,                     //   45 0x002d
        blaze,                         //   46 0x002e
        fire_ball,                     //   47 0x002f
        nova,                          //   48 0x0030
        lightning,                     //   49 0x0031
        shiver_armor,                  //   50 0x0032
        fire_wall,                     //   51 0x0033
        enchant,                       //   52 0x0034
        chain_lightning,               //   53 0x0035
        teleport,                      //   54 0x0036
        glacial_spike,                 //   55 0x0037
        meteor,                        //   56 0x0038
        thunder_storm,                 //   57 0x0039
        energy_shield,                 //   58 0x003a
        blizzard,                      //   59 0x003b
        chilling_armor,                //   60 0x003c
        fire_mastery,                  //   61 0x003d
        hydra,                         //   62 0x003e
        lightning_mastery,             //   63 0x003f
        frozen_orb,                    //   64 0x0040
        cold_mastery,                  //   65 0x0041
        amplify_damage,                //   66 0x0042
        teeth,                         //   67 0x0043
        bone_armor,                    //   68 0x0044
        skeleton_mastery,              //   69 0x0045
        raise_skeleton,                //   70 0x0046
        dim_vision,                    //   71 0x0047
        weaken,                        //   72 0x0048
        poison_dagger,                 //   73 0x0049
        corpse_explosion,              //   74 0x004a
        clay_golem,                    //   75 0x004b
        iron_maiden,                   //   76 0x004c
        terror,                        //   77 0x004d
        bone_wall,                     //   78 0x004e
        golem_mastery,                 //   79 0x004f
        raise_skeletal_mage,           //   80 0x0050
        confuse,                       //   81 0x0051
        life_tap,                      //   82 0x0052
        poison_explosion,              //   83 0x0053
        bone_spear,                    //   84 0x0054
        bloodgolem,                    //   85 0x0055
        attract,                       //   86 0x0056
        decrepify,                     //   87 0x0057
        bone_prison,                   //   88 0x0058
        summon_resist,                 //   89 0x0059
        irongolem,                     //   90 0x005a
        lower_resist,                  //   91 0x005b
        poison_nova,                   //   92 0x005c
        bone_spirit,                   //   93 0x005d
        firegolem,                     //   94 0x005e
        revive,                        //   95 0x005f
        sacrifice,                     //   96 0x0060
        smite,                         //   97 0x0061
        might,                         //   98 0x0062
        prayer,                        //   99 0x0063
        resist_fire,                   //  100 0x0064
        holy_bolt,                     //  101 0x0065
        holy_fire,                     //  102 0x0066
        thorns,                        //  103 0x0067
        defiance,                      //  104 0x0068
        resist_cold,                   //  105 0x0069
        zeal,                          //  106 0x006a
        charge,                        //  107 0x006b
        blessed_aim,                   //  108 0x006c
        cleansing,                     //  109 0x006d
        resist_lightning,              //  110 0x006e
        vengeance,                     //  111 0x006f
        blessed_hammer,                //  112 0x0070
        concentration,                 //  113 0x0071
        holy_freeze,                   //  114 0x0072
        vigor,                         //  115 0x0073
        conversion,                    //  116 0x0074
        holy_shield,                   //  117 0x0075
        holy_shock,                    //  118 0x0076
        sanctuary,                     //  119 0x0077
        meditation,                    //  120 0x0078
        fist_of_the_heavens,           //  121 0x0079
        fanaticism,                    //  122 0x007a
        conviction,                    //  123 0x007b
        redemption,                    //  124 0x007c
        salvation,                     //  125 0x007d
        bash,                          //  126 0x007e
        sword_mastery,                 //  127 0x007f
        axe_mastery,                   //  128 0x0080
        mace_mastery,                  //  129 0x0081
        howl,                          //  130 0x0082
        find_potion,                   //  131 0x0083
        leap,                          //  132 0x0084
        double_swing,                  //  133 0x0085
        pole_arm_mastery,              //  134 0x0086
        throwing_mastery,              //  135 0x0087
        spear_mastery,                 //  136 0x0088
        taunt,                         //  137 0x0089
        shout,                         //  138 0x008a
        stun,                          //  139 0x008b
        double_throw,                  //  140 0x008c
        increased_stamina,             //  141 0x008d
        find_item,                     //  142 0x008e
        leap_attack,                   //  143 0x008f
        concentrate,                   //  144 0x0090
        iron_skin,                     //  145 0x0091
        battle_cry,                    //  146 0x0092
        frenzy,                        //  147 0x0093
        increased_speed,               //  148 0x0094
        battle_orders,                 //  149 0x0095
        grim_ward,                     //  150 0x0096
        whirlwind,                     //  151 0x0097
        berserk,                       //  152 0x0098
        natural_resistance,            //  153 0x0099
        war_cry,                       //  154 0x009a
        battle_command,                //  155 0x009b
        fire_hit,                      //  156 0x009c
        unholybolt,                    //  157 0x009d
        skeletonraise,                 //  158 0x009e
        maggotegg,                     //  159 0x009f
        shamanfire,                    //  160 0x00a0
        magottup,                      //  161 0x00a1
        magottdown,                    //  162 0x00a2
        magottlay,                     //  163 0x00a3
        andrialspray,                  //  164 0x00a4
        jump,                          //  165 0x00a5
        swarm_move,                    //  166 0x00a6
        nest,                          //  167 0x00a7
        quick_strike,                  //  168 0x00a8
        vampirefireball,               //  169 0x00a9
        vampirefirewall,               //  170 0x00aa
        vampiremeteor,                 //  171 0x00ab
        gargoyletrap,                  //  172 0x00ac
        spiderlay,                     //  173 0x00ad
        vampireheal,                   //  174 0x00ae
        vampireraise,                  //  175 0x00af
        submerge,                      //  176 0x00b0
        fetishaura,                    //  177 0x00b1
        fetishinferno,                 //  178 0x00b2
        zakarumheal,                   //  179 0x00b3
        emerge,                        //  180 0x00b4
        resurrect,                     //  181 0x00b5
        bestow,                        //  182 0x00b6
        missileskill1,                 //  183 0x00b7
        monteleport,                   //  184 0x00b8
        primelightning,                //  185 0x00b9
        primebolt,                     //  186 0x00ba
        primeblaze,                    //  187 0x00bb
        primefirewall,                 //  188 0x00bc
        primespike,                    //  189 0x00bd
        primeicenova,                  //  190 0x00be
        primepoisonball,               //  191 0x00bf
        primepoisonnova,               //  192 0x00c0
        diablight,                     //  193 0x00c1
        diabcold,                      //  194 0x00c2
        diabfire,                      //  195 0x00c3
        fingermagespider,              //  196 0x00c4
        diabwall,                      //  197 0x00c5
        diabrun,                       //  198 0x00c6
        diabprison,                    //  199 0x00c7
        poisonballtrap,                //  200 0x00c8
        andypoisonbolt,                //  201 0x00c9
        hireablemissile,               //  202 0x00ca
        desertturret,                  //  203 0x00cb
        arcanetower,                   //  204 0x00cc
        monblizzard,                   //  205 0x00cd
        mosquito,                      //  206 0x00ce
        cursedballtrapright,           //  207 0x00cf
        cursedballtrapleft,            //  208 0x00d0
        monfrozenarmor,                //  209 0x00d1
        monbonearmor,                  //  210 0x00d2
        monbonespirit,                 //  211 0x00d3
        moncursecast,                  //  212 0x00d4
        hellmeteor,                    //  213 0x00d5
        regurgitatoreat,               //  214 0x00d6
        monfrenzy,                     //  215 0x00d7
        queendeath,                    //  216 0x00d8
        scroll_of_identify,            //  217 0x00d9
        book_of_identify,              //  218 0x00da
        scroll_of_townportal,          //  219 0x00db
        book_of_townportal,            //  220 0x00dc
        raven,                         //  221 0x00dd
        plague_poppy,                  //  222 0x00de
        wearwolf,                      //  223 0x00df
        shape_shifting,                //  224 0x00e0
        firestorm,                     //  225 0x00e1
        oak_sage,                      //  226 0x00e2
        summon_spirit_wolf,            //  227 0x00e3
        wearbear,                      //  228 0x00e4
        molten_boulder,                //  229 0x00e5
        arctic_blast,                  //  230 0x00e6
        cycle_of_life,                 //  231 0x00e7
        feral_rage,                    //  232 0x00e8
        maul,                          //  233 0x00e9
        eruption,                      //  234 0x00ea
        cyclone_armor,                 //  235 0x00eb
        heart_of_wolverine,            //  236 0x00ec
        summon_fenris,                 //  237 0x00ed
        rabies,                        //  238 0x00ee
        fire_claws,                    //  239 0x00ef
        twister,                       //  240 0x00f0
        vines,                         //  241 0x00f1
        hunger,                        //  242 0x00f2
        shock_wave,                    //  243 0x00f3
        volcano,                       //  244 0x00f4
        tornado,                       //  245 0x00f5
        spirit_of_barbs,               //  246 0x00f6
        summon_grizzly,                //  247 0x00f7
        fury,                          //  248 0x00f8
        armageddon,                    //  249 0x00f9
        hurricane,                     //  250 0x00fa
        fire_trauma,                   //  251 0x00fb
        claw_mastery,                  //  252 0x00fc
        psychic_hammer,                //  253 0x00fd
        tiger_strike,                  //  254 0x00fe
        dragon_talon,                  //  255 0x00ff
        shock_field,                   //  256 0x0100
        blade_sentinel,                //  257 0x0101
        quickness,                     //  258 0x0102
        fists_of_fire,                 //  259 0x0103
        dragon_claw,                   //  260 0x0104
        charged_bolt_sentry,           //  261 0x0105
        wake_of_fire_sentry,           //  262 0x0106
        weapon_block,                  //  263 0x0107
        cloak_of_shadows,              //  264 0x0108
        cobra_strike,                  //  265 0x0109
        blade_fury,                    //  266 0x010a
        fade,                          //  267 0x010b
        shadow_warrior,                //  268 0x010c
        claws_of_thunder,              //  269 0x010d
        dragon_tail,                   //  270 0x010e
        lightning_sentry,              //  271 0x010f
        inferno_sentry,                //  272 0x0110
        mind_blast,                    //  273 0x0111
        blades_of_ice,                 //  274 0x0112
        dragon_flight,                 //  275 0x0113
        death_sentry,                  //  276 0x0114
        blade_shield,                  //  277 0x0115
        venom,                         //  278 0x0116
        shadow_master,                 //  279 0x0117
        royal_strike,                  //  280 0x0118
        wake_of_destruction_sentry,    //  281 0x0119
        imp_inferno,                   //  282 0x011a
        imp_fireball,                  //  283 0x011b
        baal_taunt,                    //  284 0x011c
        baal_corpse_explode,           //  285 0x011d
        baal_monster_spawn,            //  286 0x011e
        catapult_charged_ball,         //  287 0x011f
        catapult_spike_ball,           //  288 0x0120
        suck_blood,                    //  289 0x0121
        cry_help,                      //  290 0x0122
        healing_vortex,                //  291 0x0123
        teleport_2,                    //  292 0x0124
        self_resurrect,                //  293 0x0125
        vine_attack,                   //  294 0x0126
        overseer_whip,                 //  295 0x0127
        barbs_aura,                    //  296 0x0128
        wolverine_aura,                //  297 0x0129
        oak_sage_aura,                 //  298 0x012a
        imp_fire_missile,              //  299 0x012b
        impregnate,                    //  300 0x012c
        siege_beast_stomp,             //  301 0x012d
        minionspawner,                 //  302 0x012e
        catapultblizzard,              //  303 0x012f
        catapultplague,                //  304 0x0130
        catapultmeteor,                //  305 0x0131
        boltsentry,                    //  306 0x0132
        corpsecycler,                  //  307 0x0133
        deathmaul,                     //  308 0x0134
        defense_curse,                 //  309 0x0135
        blood_mana,                    //  310 0x0136
        mon_inferno_sentry,            //  311 0x0137
        mon_death_sentry,              //  312 0x0138
        sentry_lightning,              //  313 0x0139
        fenris_rage,                   //  314 0x013a
        baal_tentacle,                 //  315 0x013b
        baal_nova,                     //  316 0x013c
        baal_inferno,                  //  317 0x013d
        baal_cold_missiles,            //  318 0x013e
        megademoninferno,              //  319 0x013f
        evilhutspawner,                //  320 0x0140
        countessfirewall,              //  321 0x0141
        impbolt,                       //  322 0x0142
        horror_arctic_blast,           //  323 0x0143
        death_sentry_ltng,             //  324 0x0144
        vinecycler,                    //  325 0x0145
        bearsmite,                     //  326 0x0146
        resurrect2,                    //  327 0x0147
        bloodlordfrenzy,               //  328 0x0148
        baal_teleport,                 //  329 0x0149
        imp_teleport,                  //  330 0x014a
        baal_clone_teleport,           //  331 0x014b
        zakarumlightning,              //  332 0x014c
        vampiremissile,                //  333 0x014d
        mephistomissile,               //  334 0x014e
        doomknightmissile,             //  335 0x014f
        roguemissile,                  //  336 0x0150
        hydramissile,                  //  337 0x0151
        necromagemissile,              //  338 0x0152
        monbow,                        //  339 0x0153
        monfirearrow,                  //  340 0x0154
        moncoldarrow,                  //  341 0x0155
        monexplodingarrow,             //  342 0x0156
        monfreezingarrow,              //  343 0x0157
        monpowerstrike,                //  344 0x0158
        succubusbolt,                  //  345 0x0159
        mephfrostnova,                 //  346 0x015a
        monicespear,                   //  347 0x015b
        shamanice,                     //  348 0x015c
        diablogeddon,                  //  349 0x015d
        delerium_change,               //  350 0x015e
        nihlathakcorpseexplosion,      //  351 0x015f
        serpentcharge,                 //  352 0x0160
        trap_nova,                     //  353 0x0161
        unholyboltex,                  //  354 0x0162
        shamanfireex,                  //  355 0x0163
        imp_fire_missile_ex            //  356 0x0164
    };
}

using skill::skill_type;
#endif