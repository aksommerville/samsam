#include "game.h"

/* Delete.
 */
 
void modal_del(struct modal *modal) {
  if (!modal) return;
  if (modal->del) modal->del(modal);
  free(modal);
}

/* New.
 */
 
struct modal *modal_new(int size) {
  if (size<(int)sizeof(struct modal)) return 0;
  struct modal *modal=calloc(1,size);
  if (!modal) return 0;
  return modal;
}

/* Trivial hooks.
 */
 
void modal_input(struct modal *modal,int input,int pvinput) {
  if (modal&&modal->input) modal->input(modal,input,pvinput);
}

void modal_update(struct modal *modal,double elapsed) {
  if (modal&&modal->update) modal->update(modal,elapsed);
}

void modal_render(struct modal *modal) {
  if (modal&&modal->render) modal->render(modal);
  else graf_fill_rect(&g.graf,0,0,FBW,FBH,0x000000ff);
}
