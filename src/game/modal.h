/* modal.h
 */
 
#ifndef MODAL_H
#define MODAL_H

struct modal {
  int defunct;
  void (*del)(struct modal *modal);
  void (*input)(struct modal *modal,int input,int pvinput);
  void (*update)(struct modal *modal,double elapsed);
  void (*render)(struct modal *modal);
};

void modal_del(struct modal *modal);
struct modal *modal_new(int size);
void modal_input(struct modal *modal,int input,int pvinput);
void modal_update(struct modal *modal,double elapsed);
void modal_render(struct modal *modal);

struct modal *modal_new_ghost();
struct modal *modal_new_gameover();

#endif
