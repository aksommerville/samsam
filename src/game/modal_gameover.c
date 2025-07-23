#include "game.h"

struct modal_gameover {
  struct modal hdr;
};

#define MODAL ((struct modal_gameover*)modal)

/* Cleanup.
 */
 
static void _gameover_del(struct modal *modal) {
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
}

/* Render.
 */
 
static void _gameover_render(struct modal *modal) {
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
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
  return modal;
}
