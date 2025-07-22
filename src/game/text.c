#include "game.h"

/* Generate label in a new texture.
 */

int generate_label(const char *src,int srcc) {
  if (!src) srcc=0; else if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  const int srcx=8,srcy=312;
  const int colw=3,rowh=5;
  const int xspacing=colw+1;
  const int yspacing=rowh+1;
  
  // Determine output dimensions and glyph count.
  int rowc=0,colc=1,srcp=0,vtxc=0;
  while (srcp<srcc) {
    const char *line=src+srcp;
    int linec=0;
    while ((srcp<srcc)&&(src[srcp++]!=0x0a)) linec++;
    if (linec>colc) colc=linec;
    // We're not going to draw 0x20 or 0x7f, or anything outside G0.
    for (;linec-->0;line++) if ((*line>0x20)&&(*line<0x7f)) vtxc++;
    rowc++;
  }
  if (!rowc) rowc=1;
  int fullw=1+colc*xspacing;
  int fullh=1+rowc*yspacing;
  
  // Make the destination texture and fill it with the background color.
  // Then if there's no glyphs, we're done.
  int texid=egg_texture_new();
  egg_texture_load_raw(texid,fullw,fullh,fullw<<2,0,0);
  egg_draw_clear(texid,0x4c2f0dff);
  if (!vtxc) return texid;
  
  // Prepare a vertex buffer.
  struct egg_draw_decal *vtxv=malloc(sizeof(struct egg_draw_decal)*vtxc);
  if (!vtxv) return texid;
  struct egg_draw_decal *vtx=vtxv;
  int vtxp=0;
  
  // Walk the string again, populating (vtxv).
  int x=1,y=1;
  for (;srcc-->0;src++) {
    if (*src==0x0a) {
      x=1;
      y+=yspacing;
      continue;
    }
    if ((*src>0x20)&&(*src<0x7f)) {
      int srccol=(*src)&15;
      int srcrow=((*src)>>4)-2;
      vtx->dstx=x;
      vtx->dsty=y;
      vtx->srcx=srcx+srccol*colw;
      vtx->srcy=srcy+srcrow*rowh;
      vtx->w=colw;
      vtx->h=rowh;
      vtx->xform=0;
      vtx++;
      if (++vtxp>=vtxc) break; // safety escape, in case i miscalculated above
    }
    x+=xspacing;
  }
  
  // Draw it, free the vertex buffer, and we're done.
  egg_draw_decal(texid,g.texid,vtxv,vtxc);
  free(vtxv);
  return texid;
}
