#include "game.h"

#define MERCH_LIMIT 24
#define MERCH_COLC 6
#define MERCH_ROWC 4
#define TOP_MARGIN 5

struct modal_ghost {
  struct modal hdr;
  struct merch {
    const struct decal *decal;
    int dstx,dsty;
    int sold;
  } merchv[MERCH_LIMIT];
  int merchc;
  int merchp;
  int coinq_texid,coinq_w,coinq_h;
};

#define MODAL ((struct modal_ghost*)modal)

/* Delete.
 */
 
static void _ghost_del(struct modal *modal) {
  egg_texture_del(MODAL->coinq_texid);
}

/* Activate.
 */
 
static void ghost_activate(struct modal *modal) {
  if ((MODAL->merchp<0)||(MODAL->merchp>=MODAL->merchc)) return;
  const struct merch *merch=MODAL->merchv+MODAL->merchp;
  if (merch->sold) {
    samsam_sound(RID_sound_reject);
  } else if (merch->decal->price>g.coinc) {
    samsam_sound(RID_sound_reject);
  } else if (merch->decal->price>0) {
    samsam_sound(RID_sound_purchase);
    g.coinc-=merch->decal->price;
    g.man.larm=MAN_ARM_SIDE;
    g.man.carry_item=merch->decal-decalv;
    g.soldv[merch->decal-decalv]=1;
    modal->defunct=1;
  } else {
    samsam_sound(RID_sound_uiaction);
    modal->defunct=1;
  }
}

/* Move cursor.
 */
 
static void ghost_move(struct modal *modal,int dx,int dy) {
  if (MODAL->merchc<1) return;
  int col=MODAL->merchp%MERCH_COLC+dx;
  int row=MODAL->merchp/MERCH_COLC+dy;
  if (col<0) col=MERCH_COLC-1; else if (col>=MERCH_COLC) col=0;
  if (row<0) row=MERCH_ROWC-1; else if (row>=MERCH_ROWC) row=0;
  int np=row*MERCH_COLC+col;
  if (np>=MODAL->merchc) np=MODAL->merchc-1;
  MODAL->merchp=np;
  samsam_sound(RID_sound_uimotion);
}

/* Input.
 */
 
static void _ghost_input(struct modal *modal,int input,int pvinput) {
  if ((input&EGG_BTN_WEST)&&!(pvinput&EGG_BTN_WEST)) {
    modal->defunct=1;
  }
  if ((input&EGG_BTN_LEFT)&&!(pvinput&EGG_BTN_LEFT)) ghost_move(modal,-1,0);
  if ((input&EGG_BTN_RIGHT)&&!(pvinput&EGG_BTN_RIGHT)) ghost_move(modal,1,0);
  if ((input&EGG_BTN_UP)&&!(pvinput&EGG_BTN_UP)) ghost_move(modal,0,-1);
  if ((input&EGG_BTN_DOWN)&&!(pvinput&EGG_BTN_DOWN)) ghost_move(modal,0,1);
  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) ghost_activate(modal);
}

/* Update.
 */
 
static void _ghost_update(struct modal *modal,double elapsed) {
}

/* Render.
 */
 
static void _ghost_render(struct modal *modal) {

  graf_fill_rect(&g.graf,0,0,FBW,FBH,0x6d79a7ff);

  const struct decal *bpk=decalv+NS_DECAL_backpack;
  graf_set_input(&g.graf,g.texid);
  graf_decal(&g.graf,(FBW>>1)-(bpk->w>>1),TOP_MARGIN,bpk->x,bpk->y,bpk->w,bpk->h);
  const struct decal *coin=decalv+NS_DECAL_cent;
  graf_decal(&g.graf,300,20,coin->x,coin->y,coin->w,coin->h);
  graf_set_input(&g.graf,MODAL->coinq_texid);
  graf_decal(&g.graf,310,20,0,0,MODAL->coinq_w,MODAL->coinq_h);

  graf_set_input(&g.graf,g.texid);
  const struct merch *merch=MODAL->merchv;
  int i=MODAL->merchc;
  for (;i-->0;merch++) {
    if (merch->sold) graf_set_alpha(&g.graf,0x40);
    graf_decal(&g.graf,merch->dstx,merch->dsty,merch->decal->x,merch->decal->y,merch->decal->w,merch->decal->h);
    if (merch->decal->price>0) {
      int dsty=merch->dsty+merch->decal->h+2;
      int coinsw=merch->decal->price*coin->w;
      int dstx=merch->dstx+(merch->decal->w>>1)-(coinsw>>1);
      int ii=merch->decal->price;
      for (;ii-->0;dstx+=coin->w) {
        graf_decal(&g.graf,dstx,dsty,coin->x,coin->y,coin->w,coin->h);
      }
    }
    if (merch->sold) graf_set_alpha(&g.graf,0xff);
  }
  if ((MODAL->merchp>=0)&&(MODAL->merchp<MODAL->merchc)&&(g.framec%40<35)) {
    merch=MODAL->merchv+MODAL->merchp;
    const int margin=2;
    const struct decal *arrow=decalv+NS_DECAL_arrow; // Natural orientation is down.
    graf_decal_xform(&g.graf,merch->dstx-margin-arrow->h,merch->dsty+(merch->decal->h>>1)-(arrow->w>>1),arrow->x,arrow->y,arrow->w,arrow->h,EGG_XFORM_SWAP);
    graf_decal_xform(&g.graf,merch->dstx+merch->decal->w+margin,merch->dsty+(merch->decal->h>>1)-(arrow->w>>1),arrow->x,arrow->y,arrow->w,arrow->h,EGG_XFORM_SWAP|EGG_XFORM_YREV);
  }
}

/* New.
 */
 
struct modal *modal_new_ghost() {
  struct modal *modal=modal_new(sizeof(struct modal_ghost));
  if (!modal) return 0;
  modal->del=_ghost_del;
  modal->input=_ghost_input;
  modal->update=_ghost_update;
  modal->render=_ghost_render;
  
  char coinq[3];
  int coinqc=0;
  if (g.coinc>=100) coinq[coinqc++]='0'+(g.coinc/100)%10;
  if (g.coinc>=10) coinq[coinqc++]='0'+(g.coinc/10)%10;
  coinq[coinqc++]='0'+g.coinc%10;
  MODAL->coinq_texid=generate_label(coinq,coinqc);
  egg_texture_get_size(&MODAL->coinq_w,&MODAL->coinq_h,MODAL->coinq_texid);
  
  // Populate the merch.
  int top=TOP_MARGIN+decalv[NS_DECAL_backpack].h;
  int colw=FBW/MERCH_COLC;
  int rowh=(FBH-top)/MERCH_ROWC;
  int col=0,row=0;
  const struct decal *decal=decalv;
  int i=0;
  for (;i<DECAL_COUNT;i++,decal++) {
    if (!decal->price) continue;
    if (MODAL->merchc>=MERCH_LIMIT) {
      fprintf(stderr,"Too many salable items\n");
      modal_del(modal);
      return 0;
    }
    struct merch *merch=MODAL->merchv+MODAL->merchc++;
    merch->decal=decal;
    merch->sold=g.soldv[i];
    int bx=col*colw,by=top+row*rowh;
    merch->dstx=bx+(colw>>1)-(decal->w>>1);
    merch->dsty=by+(rowh>>1)-(decal->h>>1);
    if (++col>=MERCH_COLC) {
      col=0;
      row++;
    }
  }
  
  // That should leave us with one slot remaining: Put the Door there.
  if (MODAL->merchc<MERCH_LIMIT) {
    struct merch *merch=MODAL->merchv+MODAL->merchc++;
    merch->decal=decalv+NS_DECAL_door;
    merch->dstx=col*colw+(colw>>1)-(merch->decal->w>>1);
    merch->dsty=top+row*rowh+(rowh>>1)-(merch->decal->h>>1);
  }
  
  return modal;
}
