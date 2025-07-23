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
#include "sky.h"

#define MAXIMUM_SCORE 999 /*TODO*/

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
  int score; // Sum of prices of items delivered to burger.
  int day; // 0,1,2,3. Game ends when it strikes 3. Burger increments in on accepting an offering.
  int item_by_day[3];
  uint8_t soldv[DECAL_COUNT];
  double playclock;
  struct modal *modal;
  struct sky sky;
  int hi_score;
  double hi_time; // "lo" time if you want to be pedantic about it.
  int new_hi_score; // Nonzero before gameover if the new score is strictly greater than the old. (score only, time doesn't count)
} g;

/* Labels are strictly monospaced. Pad with spaces if you like.
 * Lines break on LF, nowhere else.
 * There is always a 1-pixel border.
 * Returns a texture id which the caller must delete.
 */
int generate_label(const char *src,int srcc);

/* Same method as generate_label() but done live.
 * (x,y) is the top-left corner of output, and we do produce a border.
 * Single line only.
 */
void quickie_render_string(int x,int y,const char *src,int srcc);

int time_repr(char *dst,int dsta,double s);
double time_eval(const char *src,int srcc);

int load_map(int mapid);
int reset_session();

int res_get(void *dstpp,int tid,int rid);

int flag_get(int flagid);
int flag_set(int flagid,int v); // => nonzero if changed

void begin_modal_ghost();

/* Loading replaces (hi_score,hi_time) with what's stored, or sensible defaults.
 * Saving writes those to storage. Caller should replace the globals first.
 */
void hiscore_load();
void hiscore_save();

#endif
