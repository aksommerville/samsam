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
  int dp;//XXX
} g;

#endif
