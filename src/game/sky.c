#include "game.h"

#define SKY_COLOR_SUN     0xa0b0e0ff
#define SKY_COLOR_MOON    0x603080ff
#define SKY_COLOR_COOKIE  0x40a0ffff
#define SKY_COLOR_NEUTRAL 0x000000ff /* Fade to this at set, regardless of day colors. */

#define SUN_POSITION 20 /* top margin */

#define RISE_RATE 0.500 /* hz */
#define SET_RATE  0.500 /* hz */

/* Reset.
 */
 
void sky_reset(struct sky *sky) {
  sky->day=0;
  sky->sundecal=decalv+NS_DECAL_sun;
  sky->sunlevel=1.0; // Starts risen.
  sky->color=SKY_COLOR_SUN;
  sky->daytime=0.0;
}

/* Update.
 */
 
void sky_update(struct sky *sky,double elapsed) {
  sky->daytime+=elapsed;
  if (sky->day==g.day) {
    if ((sky->sunlevel+=RISE_RATE*elapsed)>=1.0) sky->sunlevel=1.0;
  } else if (g.day>2) {
    // After the cookie day ends, don't set and rise anymore.
    sky->day=g.day;
    sky->daytime=0.0;
  } else {
    if ((sky->sunlevel-=SET_RATE*elapsed)<=0.0) {
      sky->daytime=0.0;
      sky->sunlevel=0.0;
      switch (sky->day=g.day) {
        case 0: sky->sundecal=decalv+NS_DECAL_sun; sky->color=SKY_COLOR_SUN; break; // for the sake of completeness
        case 1: sky->sundecal=decalv+NS_DECAL_moon; sky->color=SKY_COLOR_MOON; break;
        case 2: default: sky->sundecal=decalv+NS_DECAL_cookie; sky->color=SKY_COLOR_COOKIE; break;
      }
    }
  }
}

/* Mix RGB.
 */
 
static uint32_t mix_color(uint32_t a,uint32_t z,double mix) {
  if (mix<=0.0) return a;
  if (mix>=1.0) return z;
  double ar=((a>>24)&0xff);
  double ag=((a>>16)&0Xff);
  double ab=((a>>8)&0xff);
  double br=((z>>24)&0xff);
  double bg=((z>>16)&0Xff);
  double bb=((z>>8)&0xff);
  int r=(int)(ar*(1.0-mix)+br*mix); if (r<0) r=0; else if (r>0xff) r=0xff;
  int g=(int)(ag*(1.0-mix)+bg*mix); if (g<0) g=0; else if (g>0xff) g=0xff;
  int b=(int)(ab*(1.0-mix)+bb*mix); if (b<0) b=0; else if (b>0xff) b=0xff;
  return (r<<24)|(g<<16)|(b<<8)|0xff;
}

/* Render.
 */
 
void sky_render(struct sky *sky) {

  uint32_t bgcolor=sky->color;
  if (sky->sunlevel<1.0) bgcolor=mix_color(SKY_COLOR_NEUTRAL,sky->color,sky->sunlevel);
  graf_draw_rect(&g.graf,0,0,FBW,FBH,bgcolor);
  
  int dstx=(FBW>>1)-(sky->sundecal->w>>1);
  int dsty=SUN_POSITION;
  if (sky->sunlevel<1.0) {
    dsty=FBH-(int)(sky->sunlevel*(FBH-SUN_POSITION));
  }
  graf_draw_decal(&g.graf,g.texid,dstx,dsty,sky->sundecal->x,sky->sundecal->y,sky->sundecal->w,sky->sundecal->h,0);
}
