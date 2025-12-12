#include "game.h"

#define BGCOLOR 0x390a20ff

/* Generate label in a new texture.
 */

int generate_label(const char *src,int srcc) {
  if (!src) srcc=0; else if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  if (srcc&&(src[srcc-1]==0x0a)) srcc--; // Trim one LF from the end if present -- assume they didn't want a blank line below.
  const int srcx=8,srcy=312;
  const int colw=3,rowh=5;
  const int xspacing=colw+1;
  const int yspacing=rowh+1;
  
  // Determine output dimensions.
  int rowc=0,colc=1,srcp=0,vtxc=0;
  while (srcp<srcc) {
    const char *line=src+srcp;
    int linec=0;
    while ((srcp<srcc)&&(src[srcp++]!=0x0a)) linec++;
    if (linec>colc) colc=linec;
    rowc++;
  }
  if (!rowc) rowc=1;
  int fullw=1+colc*xspacing;
  int fullh=1+rowc*yspacing;
  
  // Make the destination texture and fill it with the background color.
  // Then if there's no glyphs, we're done.
  graf_flush(&g.graf);
  graf_reset(&g.graf);
  int texid=egg_texture_new();
  egg_texture_load_raw(texid,fullw,fullh,fullw<<2,0,0);
  graf_set_output(&g.graf,texid);
  graf_fill_rect(&g.graf,0,0,fullw,fullh,BGCOLOR);
  graf_set_input(&g.graf,g.texid);
  
  /* Walk the string again, drawing decals.
   * This means a full round-trip for each glyph, very inefficient.
   * In Egg v1 it wasn't so bad; we could batch decals at least up to the platform layer.
   * A smarter approach, if we wanted to do this right, would be render the whole label client-side then upload it.
   * (eg what the "font" unit does).
   * But I don't want to do it right, who cares.
   */
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
      graf_decal(&g.graf,x,y,srcx+srccol*colw,srcy+srcrow*rowh,colw,rowh);
    }
    x+=xspacing;
  }
  
  graf_flush(&g.graf);
  graf_reset(&g.graf);
  return texid;
}

/* One-off text render directly to framebuffer.
 */
 
void quickie_render_string(int dstx,int dsty,const char *src,int srcc) {
  if (!src) return;
  if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  if (srcc<1) return;
  const int srcx=8,srcy=312;
  const int colw=3,rowh=5;
  int boxw=srcc*colw+srcc+1;
  int boxh=rowh+2;
  graf_fill_rect(&g.graf,dstx,dsty,boxw,boxh,BGCOLOR);
  graf_set_input(&g.graf,g.texid);
  dstx+=1;
  dsty+=1;
  for (;srcc-->0;src++,dstx+=colw+1) {
    int ch=*src;
    if (ch<=0x20) continue;
    if (ch>=0x7f) continue;
    ch-=0x20;
    int col=ch&15;
    int row=ch>>4;
    int x=srcx+col*colw;
    int y=srcy+row*rowh;
    graf_decal(&g.graf,dstx,dsty,x,y,colw,rowh);
  }
}

/* Time as "MM:SS.mmm" strings.
 */
 
int time_repr(char *dst,int dsta,double sf) {
  if (dsta<9) return 0;
  int ms=(int)(sf*1000.0);
  if (ms<0) ms=0;
  int s=ms/1000; ms%=1000;
  int m=s/60; s%=60;
  if (m>99) { m=99; s=99; ms=999; }
  dst[0]='0'+m/10;
  dst[1]='0'+m%10;
  dst[2]=':';
  dst[3]='0'+s/10;
  dst[4]='0'+s%10;
  dst[5]='.';
  dst[6]='0'+ms/100;
  dst[7]='0'+(ms/10)%10;
  dst[8]='0'+ms%10;
  return 9;
}

double time_eval(const char *src,int srcc) {
  if (!src) return 0.0;
  if (srcc<0) { srcc=0; while (src[srcc]) srcc++; }
  int m=0,s=0,ms=0,srcp=0;
  while ((srcp<srcc)&&(src[srcp]>='0')&&(src[srcp]<='9')) {
    m*=10;
    m+=src[srcp++]-'0';
  }
  if (m<0) m=0; else if (m>99) m=99;
  if ((srcp<srcc)&&(src[srcp]==':')) {
    srcp++;
    while ((srcp<srcc)&&(src[srcp]>='0')&&(src[srcp]<='9')) {
      s*=10;
      s+=src[srcp++]-'0';
    }
    if (s<0) s=0; else if (s>99) s=99;
  }
  if ((srcp<srcc)&&(src[srcp]=='.')) {
    srcp++;
    while ((srcp<srcc)&&(src[srcp]>='0')&&(src[srcp]<='9')) {
      ms*=10;
      ms+=src[srcp++]-'0';
    }
    if (ms<0) ms=0; else if (ms>999) ms=999;
  }
  return (double)(m*60+s)+(double)ms/1000.0;
}
