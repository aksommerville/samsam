#include "game.h"

/* Get resource.
 * We read serially from scratch every time.
 * I don't imagine we're hitting this enough to warrant a smarter strategy.
 */
 
int res_get(void *dstpp,int tid,int rid) {
  struct rom_reader reader;
  if (rom_reader_init(&reader,g.rom,g.romc)<0) return 0;
  struct rom_res *res;
  while (res=rom_reader_next(&reader)) {
    if (res->tid>tid) return 0;
    if (res->tid<tid) continue;
    if (res->rid>rid) return 0;
    if (res->rid<rid) continue;
    *(const void**)dstpp=res->v;
    return res->c;
  }
  return 0;
}

/* Load map, main entry point.
 */
 
int load_map(int rid) {
  
  /* Clear all volatile state.
   */
  g.man.x=g.man.y=0.0;
  g.platformc=0;
  g.spritec=0; // No cleanup required.
  g.worldw=FBW;
  g.indoors=0;
  
  /* Iterate commands.
   * Maps also have a grid, which we ignore entirely.
   */
  const void *serial=0;
  int serialc=res_get(&serial,EGG_TID_map,rid);
  struct rom_map rmap;
  if (rom_map_decode(&rmap,serial,serialc)<0) return -1;
  struct rom_command_reader reader={.v=rmap.cmdv,.c=rmap.cmdc};
  struct rom_command cmd;
  while (rom_command_reader_next(&cmd,&reader)>0) {
    switch (cmd.opcode) {
    
      case CMD_map_indoors: {
          g.indoors=1;
        } break;
    
      case CMD_map_width: {
          g.worldw=(cmd.argv[0]<<8)|cmd.argv[1];
        } break;
      
      case CMD_map_hero: {
          g.man.x=(cmd.argv[0]<<8)|cmd.argv[1];
          g.man.y=((cmd.argv[2]<<8)|cmd.argv[3])-(decalv[NS_DECAL_man].h>>1);
        } break;
      
      case CMD_map_platform: {
          if (g.platformc>=PLATFORM_LIMIT) {
            fprintf(stderr,"map:%d: Too many platforms!\n",rid);
            return -1;
          }
          struct platform *platform=g.platformv+g.platformc++;
          memset(platform,0,sizeof(struct platform));
          platform->x=(cmd.argv[0]<<8)|cmd.argv[1];
          platform->y=(cmd.argv[2]<<8)|cmd.argv[3];
          platform->w=(cmd.argv[4]<<8)|cmd.argv[5];
          platform->style=cmd.argv[6];
        } break;
        
      case CMD_map_sprite: {
          if (g.spritec>=SPRITE_LIMIT) {
            fprintf(stderr,"map:%d: Too many sprites!\n",rid);
            return -1;
          }
          if (cmd.argv[4]>=DECAL_COUNT) {
            fprintf(stderr,"map:%d: Invalid decal id %d\n",rid,cmd.argv[4]);
            return -1;
          }
          const struct decal *decal=decalv+cmd.argv[4];
          struct sprite *sprite=g.spritev+g.spritec++;
          memset(sprite,0,sizeof(struct sprite));
          sprite->x=(cmd.argv[0]<<8)|cmd.argv[1];
          sprite->y=((cmd.argv[2]<<8)|cmd.argv[3])-(decal->h>>1);
          sprite->decal=decal;
          if (sprite_init_per_map(sprite,cmd.argv[5],cmd.argv[6],cmd.argv[7])<0) {
            fprintf(stderr,"map:%d: Init sprite with decal %d failed.\n",rid,cmd.argv[4]);
            return -1;
          }
        } break;
    }
  }
  
  return 0;
}

/* Reset session.
 */
 
int reset_session() {
  memset(g.flagv,0,sizeof(g.flagv));
  g.flagv[0]=2; // NS_flag_one must always be one.
  g.coinc=0;
  g.score=0;
  g.day=0;
  sky_reset(&g.sky);
  egg_play_song(RID_song_sideshow,0,1);
  return load_map(RID_map_home);
}
