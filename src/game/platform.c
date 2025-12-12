#include "game.h"

/* Render the green bar platforms.
 */
 
static void platform_render_bar(struct platform *platform) {
  const struct decal *dedge=decalv+NS_DECAL_edge;
  const struct decal *dfill=decalv+NS_DECAL_platform;
  int x=platform->x-g.scrollx;
  int xz=x+platform->w-dedge->w; // column where the fill ends.
  graf_set_input(&g.graf,g.texid);
  graf_decal(&g.graf,x,platform->y,dedge->x,dedge->y,dedge->w,dedge->h);
  x+=dedge->w;
  while (x+dfill->w<=xz) {
    graf_decal(&g.graf,x,platform->y,dfill->x,dfill->y,dfill->w,dfill->h);
    x+=dfill->w;
  }
  graf_decal(&g.graf,x,platform->y,dfill->x,dfill->y,xz-x,dfill->h);
  graf_decal_xform(&g.graf,xz,platform->y,dedge->x,dedge->y,dedge->w,dedge->h,EGG_XFORM_XREV);
}

/* Render simpler single-decal platforms.
 */
 
static void platform_render_decal(struct platform *platform,const struct decal *decal,int dx,int dy) {
  int dstx=platform->x-g.scrollx+dx;
  int dsty=platform->y+dy;
  graf_set_input(&g.graf,g.texid);
  graf_decal_xform(&g.graf,dstx,dsty,decal->x,decal->y,decal->w,decal->h,platform->xform);
}

/* Render, dispatch.
 */
 
void platform_render(struct platform *platform) {
  switch (platform->style) {
    case NS_platform_bar: platform_render_bar(platform); break;
    case NS_platform_arrow: platform_render_decal(platform,decalv+NS_DECAL_arrow,0,0); break;
    case NS_platform_bed: platform_render_decal(platform,decalv+NS_DECAL_bed,0,-4); break;
    case NS_platform_table: platform_render_decal(platform,decalv+NS_DECAL_table,0,0); break;
    case NS_platform_chair: platform_render_decal(platform,decalv+NS_DECAL_chair,0,-10); break;
  }
}
