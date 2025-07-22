#ifndef GAME_H
#define GAME_H

#define FBW 448
#define FBH 252

#define PLATFORM_LIMIT 16

#include "egg/egg.h"
#include "opt/stdlib/egg-stdlib.h"
#include "opt/graf/graf.h"
#include "egg_rom_toc.h"
#include "shared_symbols.h"
#include "man.h"
#include "platform.h"

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
} g;

/* Labels are strictly monospaced. Pad with spaces if you like.
 * Lines break on LF, nowhere else.
 * There is always a 1-pixel border.
 * Returns a texture id which the caller must delete.
 */
int generate_label(const char *src,int srcc);

#endif
