/* Render the actual credit page using the game's native placement/mono path. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int done;
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);
 if(!done&&chtab_title40&&chtab_title50&&offscreen_surface&&pop_hd_active()){
  done=1;SDL_Surface*saved=current_target_surface;current_target_surface=offscreen_surface;
  draw_full_image(STORY_FRAME);draw_full_image(STORY_CREDITS);
  SDL_Surface*high=pop_hd_capture(offscreen_surface);
  if(!high||high->w!=1280||IMG_SavePNG(high,"build/hd-credits-page.png"))abort();
  current_target_surface=saved;
  fprintf(stderr,"HD credit page: engine placement and monochrome text rendering captured\n");
  SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);
 }
}
