/* man.h
 * Coordinates rendering a man from the one provided frame, with articulable arms and legs.
 */
 
#ifndef MAN_H
#define MAN_H

#define MAN_ARM_DOWN 0
#define MAN_ARM_SIDE 1
#define MAN_ARM_UP   2

#define MAN_WALK_SPEED 150.0

struct man {
  double x,y; // Position in world space.
  double lleg,rleg; // 0..1 = down..up.
  int larm,rarm; // MAN_ARM_*
  int walking; // If nonzero, we animate legs at update. Otherwise, we slide them back to zero.
  double dlleg,drleg; // Relevant while walking.
  int carry_item; // Zero or decalid. Rendered position depends on arms.
  double gravity;
  int seated;
  const struct platform *platform; // WEAK, owned by (g.platformv), may be null.
  double jumppower;
  int jumping; // Don't touch; use man_jump() and man_unjump().
};

void man_update(struct man *man,double elapsed);
void man_render(struct man *man);

/* The source image is horizontally symmetric, so we don't bother with a transform.
 * But when he's holding one thing in the SIDE position, it will always be his leading side.
 */
void man_face_left(struct man *man);
void man_face_right(struct man *man);

void man_jump(struct man *man);
void man_unjump(struct man *man);

void man_action(struct man *man);

#endif
