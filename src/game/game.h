#ifndef GAME_H
#define GAME_H

#define FBW 448
#define FBH 252

#define PLATFORM_LIMIT 64
#define SPRITE_LIMIT 64

#include "egg/egg.h"
#include "opt/stdlib/egg-stdlib.h"
#include "opt/graf/graf.h"
#include "opt/rom/rom.h"
#include "egg_rom_toc.h"
#include "shared_symbols.h"
#include "man.h"
#include "platform.h"
#include "sprite.h"
#include "modal.h"

extern struct g {
  void *rom;
  int romc;
  struct graf graf;
  int pvinput;
  int framec;
  int texid;
  struct man man;
  struct platform platformv[PLATFORM_LIMIT];
  int platformc;
  struct sprite spritev[SPRITE_LIMIT];
  int spritec;
  int scrollx;
  double worldw;
  int indoors;
  uint8_t flagv[32];
  int coinc;
  int score;
  struct modal *modal;
} g;

/* Labels are strictly monospaced. Pad with spaces if you like.
 * Lines break on LF, nowhere else.
 * There is always a 1-pixel border.
 * Returns a texture id which the caller must delete.
 */
int generate_label(const char *src,int srcc);

int load_map(int mapid);
int reset_session();

int res_get(void *dstpp,int tid,int rid);

int flag_get(int flagid);
int flag_set(int flagid,int v); // => nonzero if changed

void begin_modal_ghost();

#endif
