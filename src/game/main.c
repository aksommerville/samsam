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
  
  if (reset_session()<0) return -1;
  
  //g.man.rarm=MAN_ARM_SIDE;
  //g.man.carry_item=NS_DECAL_ax;
  //flag_set(NS_flag_burgerking,1);//XXX
  
  return 0;
}

/* If the hero is standing on a door, enter it.
 * This may reload the map.
 */
 
static void check_doors() {
  const struct sprite *sprite=g.spritev;
  int i=g.spritec;
  for (;i-->0;sprite++) {
  
    // Door.
    if (sprite->decal==decalv+NS_DECAL_door) {
      double dx=g.man.x-sprite->x;
      if ((dx<-10.0)||(dx>10.0)) continue;
      double dy=g.man.y-sprite->y;
      if ((dy<-10.0)||(dy>10.0)) continue;
      if (load_map(sprite->iv[0])<0) {
        fprintf(stderr,"Failed to load map:%d\n",sprite->iv[0]);
        egg_terminate(1);
      }
      return;
    
    // Ghost.
    } else if (sprite->decal==decalv+NS_DECAL_ghost) {
      if (g.man.carry_item) continue; // Only when empty-handed.
      double dx=g.man.x-sprite->x;
      if ((dx<-20.0)||(dx>20.0)) continue;
      double dy=g.man.y-sprite->y;
      if ((dy<-10.0)||(dy>10.0)) continue;
      begin_modal_ghost();
      return;
    }
  }
}

/* Update.
 */

void egg_client_update(double elapsed) {
  //TODO modals

  /* React to input changes.
   */
  int input=egg_input_get_one(0);
  if (input!=g.pvinput) {
    if (g.modal) {
      modal_input(g.modal,input,g.pvinput);
    } else {
      if ((input&EGG_BTN_SOUTH)&&!(g.pvinput&EGG_BTN_SOUTH)) man_jump(&g.man);
      else if (!(input&EGG_BTN_SOUTH)&&(g.pvinput&EGG_BTN_SOUTH)) man_unjump(&g.man);
      if ((input&EGG_BTN_WEST)&&!(g.pvinput&EGG_BTN_WEST)) man_action(&g.man);
      else if (!(input&EGG_BTN_WEST)&&(g.pvinput&EGG_BTN_WEST)) man_unaction(&g.man);
      if ((input&EGG_BTN_UP)&&!(g.pvinput&EGG_BTN_UP)) check_doors();
    }
    if ((input&EGG_BTN_AUX3)&&!(g.pvinput&EGG_BTN_AUX3)) egg_terminate(0);
    g.pvinput=input;
  }
  
  if (g.modal) {
    modal_update(g.modal,elapsed);
    if (g.modal&&g.modal->defunct) {
      modal_del(g.modal);
      g.modal=0;
    } else {
      return;
    }
  }
  
  /* Move the hero ourselves, then call out for animation, gravity, jumping, etc.
   */
  switch (input&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: {
        g.man.walking=1;
        g.man.x-=MAN_WALK_SPEED*elapsed;
        man_face_left(&g.man);
      } break;
    case EGG_BTN_RIGHT: {
        g.man.walking=1;
        g.man.x+=MAN_WALK_SPEED*elapsed;
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

/* Render game.
 */
 
static void render_game() {
  
  // Establish camera position. It only moves horizontally, and clamps to the edges.
  int worldw=(int)g.worldw;
  if (worldw<=FBW) {
    g.scrollx=(worldw>>1)-(FBW>>1);
  } else {
    g.scrollx=(int)g.man.x-(FBW>>1);
    if (g.scrollx<0) g.scrollx=0;
    else if (g.scrollx>worldw-FBW) g.scrollx=worldw-FBW;
  }
  
  // Draw the sky, and if indoors, the brick border.
  if (g.indoors) {
    graf_draw_rect(&g.graf,0,0,FBW,FBH,0xd0c094ff);
    const struct decal *decal=decalv+NS_DECAL_dollar;
    int x=0;
    for (;x<FBW;x+=decal->w) {
      graf_draw_decal(&g.graf,g.texid,x,0,decal->x,decal->y,decal->w,decal->h,0);
      graf_draw_decal(&g.graf,g.texid,x,FBH-decal->h,decal->x,decal->y,decal->w,decal->h,0);
    }
    int y=0;
    for (;y<FBH;y+=decal->h) {
      graf_draw_decal(&g.graf,g.texid,0,y,decal->x,decal->y,decal->w,decal->h,0);
      graf_draw_decal(&g.graf,g.texid,FBW-decal->w,y,decal->x,decal->y,decal->w,decal->h,0);
    }
  } else {
    graf_draw_rect(&g.graf,0,0,FBW,FBH,0xa0b0e0ff);//TODO sky
  }
  
  struct platform *platform=g.platformv;
  int i=g.platformc;
  for (;i-->0;platform++) platform_render(platform);
  
  // Sprites can't be defunct at this point, render them all.
  struct sprite *sprite=g.spritev;
  for (i=g.spritec;i-->0;sprite++) sprite_render(sprite);
  
  man_render(&g.man);
}

/* Render.
 */

void egg_client_render() {
  g.framec++;
  graf_reset(&g.graf);
  if (g.modal) {
    modal_render(g.modal);
  } else {
    render_game();
  }
  graf_flush(&g.graf);
}

/* Flags.
 */
 
int flag_get(int flagid) {
  if (flagid<0) return 0;
  int p=flagid>>3;
  if (p>=sizeof(g.flagv)) return 0;
  return (g.flagv[p]&(1<<(flagid&7)))?1:0;
}

int flag_set(int flagid,int v) {
  if (flagid<2) return 0; // Flags zero and one are read-only.
  int p=flagid>>3;
  if (p>=sizeof(g.flagv)) return 0;
  uint8_t mask=1<<(flagid&7);
  if (v) {
    if (g.flagv[p]&mask) return 0;
    g.flagv[p]|=mask;
  } else {
    if (!(g.flagv[p]&mask)) return 0;
    g.flagv[p]&=~mask;
  }
  return 1;
}

/* Modals.
 */
 
void begin_modal_ghost() {
  if (g.modal) modal_del(g.modal);
  g.modal=modal_new_ghost();
}
