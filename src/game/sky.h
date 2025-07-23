/* sky.h
 * Manages rendering the background of outdoor levels.
 */
 
#ifndef SKY_H
#define SKY_H

struct sky {
  int day; // Tracks (g.day) but lags behind. This is the real effective day.
  const struct decal *sundecal; // sun, moon, or cookie
  double sunlevel; // 0..1 = set..risen
  uint32_t color;
  double daytime; // Counts up, resets at sky's transition (ie when set)
};

void sky_reset(struct sky *sky);
void sky_update(struct sky *sky,double elapsed);
void sky_render(struct sky *sky);

#endif
