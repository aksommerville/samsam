#include "game.h"

/* Render.
 */
 
void platform_render(struct platform *platform) {
  //TODO Some platforms will be arrows stuck in trees, not the green bar.
  const struct decal *dedge=decalv+DECAL_edge;
  const struct decal *dfill=decalv+DECAL_platform;
  int x=platform->x-g.scrollx;
  int xz=x+platform->w-dedge->w; // column where the fill ends.
  graf_draw_decal(&g.graf,g.texid,x,platform->y,dedge->x,dedge->y,dedge->w,dedge->h,0);
  x+=dedge->w;
  while (x+dfill->w<=xz) {
    graf_draw_decal(&g.graf,g.texid,x,platform->y,dfill->x,dfill->y,dfill->w,dfill->h,0);
    x+=dfill->w;
  }
  graf_draw_decal(&g.graf,g.texid,x,platform->y,dfill->x,dfill->y,xz-x,dfill->h,0);
  graf_draw_decal(&g.graf,g.texid,xz,platform->y,dedge->x,dedge->y,dedge->w,dedge->h,EGG_XFORM_XREV);
}
