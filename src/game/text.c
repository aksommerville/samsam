#include "game.h"

/* Generate label in a new texture.
 */

int generate_label(const char *src,int srcc) {
  if (!src) srcc=0; else if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  #define VTX_LIMIT 128
  struct egg_draw_decal vtxv[VTX_LIMIT];
  int vtxc=0;
  if (srcc>VTX_LIMIT) srcc=VTX_LIMIT;
  const int srcx=8,srcy=312;
  const int colw=3,rowh=5;
  const int xspacing=colw+1;
  const int h=rowh+2;
  int w=1+srcc*xspacing;
  int texid=egg_texture_new();
  egg_texture_load_raw(texid,w,h,w<<2,0,0);
  egg_draw_clear(texid,0x1b3885ff);
  int x=1;
  for (;srcc-->0;src++,x+=xspacing) {
    int srccol=(*src)&15;
    int srcrow=(*src)>>4;
    srcrow-=2; // we've only got rows 2 thru 7
    if ((srcrow<0)||(srcrow>=6)) continue;
    int sx=srcx+srccol*colw;
    int sy=srcy+srcrow*rowh;
    struct egg_draw_decal *vtx=vtxv+vtxc++;
    vtx->dstx=x;
    vtx->dsty=1;
    vtx->srcx=sx;
    vtx->srcy=sy;
    vtx->w=colw;
    vtx->h=rowh;
    vtx->xform=0;
  }
  egg_draw_decal(texid,g.texid,vtxv,vtxc);
  return texid;
  #undef VTX_LIMIT
}
