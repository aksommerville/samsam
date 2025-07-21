/* shared_symbols.h
 * Consumed by both the game and the tools.
 */

#ifndef SHARED_SYMBOLS_H
#define SHARED_SYMBOLS_H

#define NS_sys_tilesize 16
#define NS_sys_mapw 20
#define NS_sys_maph 11

#define CMD_map_image     0x20 /* u16:imageid */
#define CMD_map_neighbors 0x60 /* u16:left u16:right u16:up u16:down */
#define CMD_map_sprite    0x61 /* u16:pos u16:spriteid u32:reserved */
#define CMD_map_door      0x62 /* u16:pos u16:mapid u16:dstpos u16:reserved */

#define CMD_sprite_image  0x20 /* u16:imageid */
#define CMD_sprite_tile   0x21 /* u8:tileid u8:xform */

#define NS_tilesheet_physics     1
#define NS_tilesheet_neighbors   0
#define NS_tilesheet_family      0
#define NS_tilesheet_weight      0

// All the individual decals provided. These are the only graphics I will use.
// Note that decal zero is empty and unused: I want nonzero IDs for the valid ones.
#define DECAL_dummy 0
#define DECAL_mattock 1
#define DECAL_shield 2
#define DECAL_sword 3
#define DECAL_shotgun 4
#define DECAL_bomb 5
#define DECAL_bow 6
#define DECAL_arrow 7
#define DECAL_speck 8
#define DECAL_ax 9
#define DECAL_bed 10
#define DECAL_basketball 11
#define DECAL_umbrella 12
#define DECAL_man 13
#define DECAL_door 14
#define DECAL_flowerpot 15
#define DECAL_table 16
#define DECAL_foot 17
#define DECAL_chair 18
#define DECAL_backpack 19
#define DECAL_tower 20
#define DECAL_workstation 21
#define DECAL_balloon 22
#define DECAL_bottle 23
#define DECAL_cent 24
#define DECAL_skull 25
#define DECAL_heart 26
#define DECAL_page 27
#define DECAL_edge 28
#define DECAL_platform 29
#define DECAL_hourglass 30
#define DECAL_dollar 31
#define DECAL_magnifier 32
#define DECAL_crown 33
#define DECAL_scroll 34
#define DECAL_blob 35
#define DECAL_bug 36
#define DECAL_widemouth 37
#define DECAL_house 38
#define DECAL_milk 39
#define DECAL_puffball 40
#define DECAL_ivy 41
#define DECAL_earcup 42
#define DECAL_pottedplant 43
#define DECAL_book 44
#define DECAL_paddle 45
#define DECAL_cookie 46
#define DECAL_flatblob 47
#define DECAL_earth 48
#define DECAL_weed 49
#define DECAL_weed2 50
#define DECAL_log 51
#define DECAL_moon 52
#define DECAL_pine 53
#define DECAL_sun 54
#define DECAL_COUNT 55
extern const struct decal {
  int x,y,w,h;
} decalv[DECAL_COUNT];

#endif
