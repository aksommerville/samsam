#include "game.h"

/* Render.
 */
 
void platform_render(struct platform *platform) {
  const struct decal *dedge=decalv+DECAL_edge;
  const struct decal *dfill=decalv+DECAL_platform;
  int xz=platform->x+platform->w-dedge->w; // column where the fill ends.
  int x=platform->x;
  graf_draw_decal(&g.graf,g.texid,x,platform->y,dedge->x,dedge->y,dedge->w,dedge->h,0);
  x+=dedge->w;
  while (x+dfill->w<=xz) {
    graf_draw_decal(&g.graf,g.texid,x,platform->y,dfill->x,dfill->y,dfill->w,dfill->h,0);
    x+=dfill->w;
  }
  graf_draw_decal(&g.graf,g.texid,x,platform->y,dfill->x,dfill->y,xz-x,dfill->h,0);
  graf_draw_decal(&g.graf,g.texid,xz,platform->y,dedge->x,dedge->y,dedge->w,dedge->h,EGG_XFORM_XREV);
}
