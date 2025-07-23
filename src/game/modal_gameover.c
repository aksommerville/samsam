#include "game.h"

#define ITEM_SPACING 8
#define ITEM_TIME_1 2.000
#define ITEM_TIME_2 2.750
#define ITEM_TIME_3 3.500
#define EARTH_Y 25
#define BURGER_TIME 1.000

struct modal_gameover {
  struct modal hdr;
  int rptid,rptw,rpth;
  double clock;
  struct item {
    const struct decal *decal;
    int dstx,dsty;
  } itemv[3];
  int itemc; // How many are displayed right now. The set is always fully populated.
  struct sprite burger;
};

#define MODAL ((struct modal_gameover*)modal)

/* Cleanup.
 */
 
static void _gameover_del(struct modal *modal) {
  egg_texture_del(MODAL->rptid);
}

/* Input.
 */
 
static void _gameover_input(struct modal *modal,int input,int pvinput) {
  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) {
    modal->defunct=1;
    reset_session();
  }
}

/* Update.
 */
 
static void _gameover_update(struct modal *modal,double elapsed) {
  MODAL->clock+=elapsed;
  
  // After a second elapses, start updating the burger.
  if (MODAL->burger.update&&(MODAL->clock>=BURGER_TIME)) {
    MODAL->burger.update(&MODAL->burger,elapsed);
  }
  
  // Update (itemc) per (clock).
  if ((MODAL->itemc<3)&&(MODAL->clock>=ITEM_TIME_3)) {
    egg_play_sound(RID_sound_uimotion);
    MODAL->itemc=3;
  } else if ((MODAL->itemc<2)&&(MODAL->clock>=ITEM_TIME_2)) {
    egg_play_sound(RID_sound_uimotion);
    MODAL->itemc=2;
  } else if ((MODAL->itemc<1)&&(MODAL->clock>=ITEM_TIME_1)) {
    egg_play_sound(RID_sound_uimotion);
    MODAL->itemc=1;
  }
}

/* Render.
 */
 
static void _gameover_render(struct modal *modal) {

  // The infinite blackness of space. But more of an infinite dark gray, so the sprites' outlines show.
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0x302438ff);
  
  // Earth.
  {
    const struct decal *decal=decalv+NS_DECAL_earth;
    graf_draw_decal(&g.graf,g.texid,(FBW>>1)-(decal->w>>1),EARTH_Y,decal->x,decal->y,decal->w,decal->h,0);
  }
  
  // Burger.
  if (MODAL->burger.render&&(MODAL->clock>=BURGER_TIME)) {
    MODAL->burger.render(&MODAL->burger);
  }
  
  // The delivered items, appearing on a timer.
  const struct item *item=MODAL->itemv;
  int i=MODAL->itemc;
  for (;i-->0;item++) {
    graf_draw_decal(&g.graf,g.texid,item->dstx,item->dsty,item->decal->x,item->decal->y,item->decal->w,item->decal->h,0);
  }
  
  // Final report, sneakily positioned to occlude our font image, in the equatorial Pacific.
  graf_draw_decal(&g.graf,MODAL->rptid,110,109,0,0,MODAL->rptw,MODAL->rpth,0);
}

/* Populate one of our item records.
 * Guaranteed to set (decal); dummy if invalid.
 */
 
static void populate_item(struct item *item,int decalid) {
  if ((decalid<0)||(decalid>=DECAL_COUNT)) decalid=0;
  item->decal=decalv+decalid;
}

/* New.
 */
 
struct modal *modal_new_gameover() {
  struct modal *modal=modal_new(sizeof(struct modal_gameover));
  if (!modal) return 0;
  modal->del=_gameover_del;
  modal->input=_gameover_input;
  modal->update=_gameover_update;
  modal->render=_gameover_render;
  egg_play_song(0/*RID_song_gameover*/,0,1);
  
  /* Compose the report.
   * This should occupy at least 16x6 cells*, so it can cover the bit of the earth decal that we're using as a font.
   * [*] Can be a bit less, since the source image has no spacing. But it's no problem to go too big.
   */
  char rpt[1024];
  int rptc=0;
  #define APPEND_LITERAL(s) { \
    int addc=sizeof(s)-1; \
    if (rptc<=sizeof(rpt)-addc) { \
      memcpy(rpt+rptc,s,addc); \
      rptc+=addc; \
    } \
  }
  #define APPEND_DECIMAL(n) { \
    int _n=(n); \
    if ((rptc<sizeof(rpt))&&(_n>=100000)) rpt[rptc++]='0'+(_n/100000)%10; \
    if ((rptc<sizeof(rpt))&&(_n>=10000 )) rpt[rptc++]='0'+(_n/10000 )%10; \
    if ((rptc<sizeof(rpt))&&(_n>=1000  )) rpt[rptc++]='0'+(_n/1000  )%10; \
    if ((rptc<sizeof(rpt))&&(_n>=100   )) rpt[rptc++]='0'+(_n/100   )%10; \
    if ((rptc<sizeof(rpt))&&(_n>=10    )) rpt[rptc++]='0'+(_n/10    )%10; \
    if (rptc<sizeof(rpt)) rpt[rptc++]='0'+_n%10; \
  }
  APPEND_LITERAL("You win!\n")
  APPEND_LITERAL("\n")
  APPEND_LITERAL("By AK Sommerville\n")
  APPEND_LITERAL("For Same Same Game Jam\n")
  APPEND_LITERAL("26 July 2025\n")
  APPEND_LITERAL("\n")
  APPEND_LITERAL("Score: ")
  APPEND_DECIMAL(g.score)
  APPEND_LITERAL("\n")
  APPEND_LITERAL("Time: ")
  rptc+=time_repr(rpt+rptc,sizeof(rpt)-rptc,g.playclock);
  APPEND_LITERAL("\n")
  APPEND_LITERAL("High: ")
  APPEND_DECIMAL(g.hi_score)
  APPEND_LITERAL(" / ")
  rptc+=time_repr(rpt+rptc,sizeof(rpt)-rptc,g.hi_time);
  APPEND_LITERAL("\n")
  if (g.score>=MAXIMUM_SCORE) {
    APPEND_LITERAL("Perfect score!!!\n")
  } else if (g.new_hi_score) {
    APPEND_LITERAL("New high score!!!\n")
  } else {
    APPEND_LITERAL("Can you reach ")
    APPEND_DECIMAL(MAXIMUM_SCORE)
    APPEND_LITERAL("?\n")
  }
  #undef APPEND_LITERAL
  #undef APPEND_DECIMAL
  MODAL->rptid=generate_label(rpt,rptc);
  egg_texture_get_status(&MODAL->rptw,&MODAL->rpth,MODAL->rptid);
  
  /* Fill (itemv) with the three items we delivered, and position them below where Earth will go.
   */
  populate_item(MODAL->itemv+0,g.item_by_day[0]);
  populate_item(MODAL->itemv+1,g.item_by_day[1]);
  populate_item(MODAL->itemv+2,g.item_by_day[2]);
  int itemsw=MODAL->itemv[0].decal->w+MODAL->itemv[1].decal->w+MODAL->itemv[2].decal->w+ITEM_SPACING*2;
  MODAL->itemv[0].dstx=(FBW>>1)-(itemsw>>1);
  MODAL->itemv[1].dstx=MODAL->itemv[0].dstx+MODAL->itemv[0].decal->w+ITEM_SPACING;
  MODAL->itemv[2].dstx=MODAL->itemv[1].dstx+MODAL->itemv[1].decal->w+ITEM_SPACING;
  int itemsy=EARTH_Y+decalv[NS_DECAL_earth].h;
  int itemsh=FBH-itemsy;
  MODAL->itemv[0].dsty=itemsy+(itemsh>>1)-(MODAL->itemv[0].decal->h>>1);
  MODAL->itemv[1].dsty=itemsy+(itemsh>>1)-(MODAL->itemv[1].decal->h>>1);
  MODAL->itemv[2].dsty=itemsy+(itemsh>>1)-(MODAL->itemv[2].decal->h>>1);
  
  /* Make a sprite for the burger.
   * It's a real sprite, but in our own storage.
   * Need to cheat off the global scroll position, so burger appears in the right place. (it gets wiped when we dismiss anyway).
   * Also, cheat the hero off to the right so the burger faces right always.
   */
  MODAL->burger.decal=decalv+NS_DECAL_burger;
  MODAL->burger.x=270.0;
  MODAL->burger.y=75.0;
  g.scrollx=0;
  g.man.x=FBW*2;
  sprite_init_per_map(&MODAL->burger,0,0,1); // '1' puts it in 'gameover' mode.
  
  return modal;
}
