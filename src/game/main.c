#include "game.h"

struct g g={0};

void egg_client_quit(int status) {
}

int egg_client_init() {

  int fbw=0,fbh=0;  
  egg_texture_get_status(&fbw,&fbh,1);
  if ((fbw!=FBW)||(fbh!=FBH)) return -1;
  
  if ((g.romc=egg_get_rom(0,0))<=0) return -1;
  if (!(g.rom=malloc(g.romc))) return -1;
  if (egg_get_rom(g.rom,g.romc)!=g.romc) return -1;
  
  srand_auto();
  egg_texture_load_image(g.texid=egg_texture_new(),RID_image_graphics);
  
  g.man.x=FBW>>1;
  g.man.y=FBH>>1;
  //g.man.carry_item=D_Equipment_bomb;
  
  return 0;
}

void egg_client_update(double elapsed) {

  int input=egg_input_get_one(0);
  if (input!=g.pvinput) {
    if ((input&EGG_BTN_SOUTH)&&!(g.pvinput&EGG_BTN_SOUTH)) switch (g.man.larm) {
      case MAN_ARM_DOWN: g.man.larm=MAN_ARM_SIDE; break;
      case MAN_ARM_SIDE: g.man.larm=MAN_ARM_UP; break;
      default: g.man.larm=MAN_ARM_DOWN; break;
    }
    if ((input&EGG_BTN_WEST)&&!(g.pvinput&EGG_BTN_WEST)) switch (g.man.rarm) {
      case MAN_ARM_DOWN: g.man.rarm=MAN_ARM_SIDE; break;
      case MAN_ARM_SIDE: g.man.rarm=MAN_ARM_UP; break;
      default: g.man.rarm=MAN_ARM_DOWN; break;
    }
    if ((input&EGG_BTN_AUX3)&&!(g.pvinput&EGG_BTN_AUX3)) egg_terminate(0);
    g.pvinput=input;
  }
  
  switch (input&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: {
        g.man.walking=1;
        g.man.x-=MAN_WALK_SPEED*elapsed;
      } break;
    case EGG_BTN_RIGHT: {
        g.man.walking=1;
        g.man.x+=MAN_WALK_SPEED*elapsed;
      } break;
    default: g.man.walking=0;
  }
  switch (input&(EGG_BTN_UP|EGG_BTN_DOWN)) {
    case EGG_BTN_UP: {
        g.man.walking=1;
        g.man.y-=MAN_WALK_SPEED*elapsed;
      } break;
    case EGG_BTN_DOWN: {
        g.man.walking=1;
        g.man.y+=MAN_WALK_SPEED*elapsed;
      } break;
  }
  man_update(&g.man,elapsed);
}

void egg_client_render() {
  g.framec++;
  graf_reset(&g.graf);
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0xf9f6c7ff);
  
  man_render(&g.man);
  
  graf_flush(&g.graf);
}
