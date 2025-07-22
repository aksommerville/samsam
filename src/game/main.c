#include "game.h"

struct g g={0};

void egg_client_quit(int status) {
}

/* Init.
 */

int egg_client_init() {

  int fbw=0,fbh=0;  
  egg_texture_get_status(&fbw,&fbh,1);
  if ((fbw!=FBW)||(fbh!=FBH)) return -1;
  
  if ((g.romc=egg_get_rom(0,0))<=0) return -1;
  if (!(g.rom=malloc(g.romc))) return -1;
  if (egg_get_rom(g.rom,g.romc)!=g.romc) return -1;
  
  srand_auto();
  egg_texture_load_image(g.texid=egg_texture_new(),RID_image_graphics);
  
  g.man.x=FBW>>1;//XXX
  g.man.y=FBH>>1;
  g.man.carry_item=DECAL_bow;
  g.man.larm=MAN_ARM_SIDE;
  
  g.platformv[g.platformc++]=(struct platform){ 10,100, 81};//XXX
  g.platformv[g.platformc++]=(struct platform){110,140, 61};
  g.platformv[g.platformc++]=(struct platform){ 50, 80, 81};
  g.platformv[g.platformc++]=(struct platform){300,100,101};
  g.worldw=FBW;
  
  return 0;
}

/* Update.
 */

void egg_client_update(double elapsed) {
  //TODO modals

  /* React to input changes.
   */
  int input=egg_input_get_one(0);
  if (input!=g.pvinput) {
    if ((input&EGG_BTN_EAST)&&!(g.pvinput&EGG_BTN_EAST)) { //XXX
      g.man.y=20.0;
      g.man.seated=0;
    }
    if ((input&EGG_BTN_SOUTH)&&!(g.pvinput&EGG_BTN_SOUTH)) man_jump(&g.man);
    else if (!(input&EGG_BTN_SOUTH)&&(g.pvinput&EGG_BTN_SOUTH)) man_unjump(&g.man);
    if ((input&EGG_BTN_WEST)&&!(g.pvinput&EGG_BTN_WEST)) man_action(&g.man);
    else if (!(input&EGG_BTN_WEST)&&(g.pvinput&EGG_BTN_WEST)) man_unaction(&g.man);
    if ((input&EGG_BTN_AUX3)&&!(g.pvinput&EGG_BTN_AUX3)) egg_terminate(0);
    g.pvinput=input;
  }
  
  /* Move the hero ourselves, then call out for animation, gravity, jumping, etc.
   */
  switch (input&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: {
        g.man.walking=1;
        g.man.x-=MAN_WALK_SPEED*elapsed;
        if (g.man.x<0.0) g.man.x=0.0;
        man_face_left(&g.man);
      } break;
    case EGG_BTN_RIGHT: {
        g.man.walking=1;
        g.man.x+=MAN_WALK_SPEED*elapsed;
        if (g.man.x>g.worldw) g.man.x=g.worldw;
        man_face_right(&g.man);
      } break;
    default: g.man.walking=0;
  }
  man_update(&g.man,elapsed);
  
  /* Update sprites, then remove defunct ones.
   */
  struct sprite *sprite=g.spritev;
  int i=g.spritec;
  for (;i-->0;sprite++) {
    if (!sprite->update) continue;
    if (sprite->defunct) continue;
    sprite->update(sprite,elapsed);
  }
  for (i=g.spritec,sprite=g.spritev+g.spritec-1;i-->0;sprite--) {
    if (!sprite->defunct) continue;
    g.spritec--;
    memmove(sprite,sprite+1,sizeof(struct sprite)*(g.spritec-i));
  }
}

/* Render.
 */

void egg_client_render() {
  g.framec++;
  graf_reset(&g.graf);
  
  //TODO modals
  
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0xa0b0e0ff);//TODO sky
  
  struct platform *platform=g.platformv;
  int i=g.platformc;
  for (;i-->0;platform++) platform_render(platform);
  
  // Sprites can't be defunct at this point, render them all.
  struct sprite *sprite=g.spritev;
  for (i=g.spritec;i-->0;sprite++) sprite_render(sprite);
  
  man_render(&g.man);
  
  graf_flush(&g.graf);
}
