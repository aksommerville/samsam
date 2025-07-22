/* shared_symbols.h
 * Consumed by both the game and the tools.
 */

#ifndef SHARED_SYMBOLS_H
#define SHARED_SYMBOLS_H

#define NS_sys_tilesize 16
#define NS_sys_mapw 20
#define NS_sys_maph 11

#define CMD_map_indoors   0x01
#define CMD_map_width     0x20 /* u16:w(pixels) */
#define CMD_map_hero      0x40 /* u16:x u16:y(foot) */
#define CMD_map_platform  0x60 /* u16:x u16:y(top) u16:w u8:style(NS_platform_) u8:reserved */
#define CMD_map_sprite    0x61 /* u16:x u16:y(foot) u8:decalid u24:args */

#define CMD_sprite_image  0x20 /* u16:imageid */
#define CMD_sprite_tile   0x21 /* u8:tileid u8:xform */

#define NS_tilesheet_physics     1
#define NS_tilesheet_neighbors   0
#define NS_tilesheet_family      0
#define NS_tilesheet_weight      0

#define NS_platform_bar 0
#define NS_platform_arrow 1
#define NS_platform_bed 2
#define NS_platform_table 3
#define NS_platform_chair 4

#define NS_flag_zero 0
#define NS_flag_one 1
#define NS_flag_tree1 2
#define NS_flag_burgerking 3
#define NS_flag_coin001 4
#define NS_flag_coin002 5
#define NS_flag_coin003 6
#define NS_flag_coin004 7
#define NS_flag_coin005 8
#define NS_flag_coin006 9
#define NS_flag_coin007 10
#define NS_flag_coin008 11
#define NS_flag_coin009 12
#define NS_flag_coin010 13

// All the individual decals provided. These are the only graphics I will use.
// Note that decal zero is empty and unused: I want nonzero IDs for the valid ones.
#define NS_DECAL_dummy        0
#define NS_DECAL_mattock      1
#define NS_DECAL_shield       2
#define NS_DECAL_sword        3
#define NS_DECAL_shotgun      4
#define NS_DECAL_bomb         5
#define NS_DECAL_bow          6
#define NS_DECAL_arrow        7
#define NS_DECAL_speck        8
#define NS_DECAL_ax           9
#define NS_DECAL_bed         10
#define NS_DECAL_basketball  11
#define NS_DECAL_umbrella    12
#define NS_DECAL_man         13
#define NS_DECAL_door        14
#define NS_DECAL_flowerpot   15
#define NS_DECAL_table       16
#define NS_DECAL_foot        17
#define NS_DECAL_chair       18
#define NS_DECAL_backpack    19
#define NS_DECAL_tower       20
#define NS_DECAL_workstation 21
#define NS_DECAL_balloon     22
#define NS_DECAL_bottle      23
#define NS_DECAL_cent        24
#define NS_DECAL_skull       25
#define NS_DECAL_heart       26
#define NS_DECAL_page        27
#define NS_DECAL_edge        28
#define NS_DECAL_platform    29
#define NS_DECAL_hourglass   30
#define NS_DECAL_dollar      31
#define NS_DECAL_magnifier   32
#define NS_DECAL_crown       33
#define NS_DECAL_scroll      34
#define NS_DECAL_dog         35
#define NS_DECAL_candy       36
#define NS_DECAL_widemouth   37
#define NS_DECAL_house       38
#define NS_DECAL_milk        39
#define NS_DECAL_burger      40
#define NS_DECAL_ivy         41
#define NS_DECAL_earcup      42
#define NS_DECAL_pottedplant 43
#define NS_DECAL_book        44
#define NS_DECAL_paddle      45
#define NS_DECAL_cookie      46
#define NS_DECAL_ghost       47
#define NS_DECAL_earth       48
#define NS_DECAL_weed        49
#define NS_DECAL_weed2       50
#define NS_DECAL_log         51
#define NS_DECAL_moon        52
#define NS_DECAL_pine        53
#define NS_DECAL_sun         54
#define DECAL_COUNT 55
extern const struct decal {
  int x,y,w,h;
  int flop; // -1 to flop in left hand, 1 in right, 0 none.
  int holddx,holddy; // Offset when held in the right hand (reverse dx in left).
  int price; // How many coins to buy from ghost? If zero, not for sale.
} decalv[DECAL_COUNT];

#endif
