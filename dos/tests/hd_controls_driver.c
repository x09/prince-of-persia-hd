/* Input/render integration checks, loaded only for HD preview tests. */
#define _GNU_SOURCE
#include <SDL.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned frames,events,hd,dos,mirrored,zoomed,checks,failed;
static void verify(const char *title,const char *expected) {
 ++checks;if(!strstr(title,expected)){++failed;fprintf(stderr,"HD control mismatch: expected %s, got %s\n",expected,title);}
}
int SDL_RenderCopyEx(SDL_Renderer *r,SDL_Texture *t,const SDL_Rect *s,const SDL_Rect *d,double a,const SDL_Point *c,SDL_RendererFlip f) {
 int (*real_copy)(SDL_Renderer *,SDL_Texture *,const SDL_Rect *,const SDL_Rect *,double,const SDL_Point *,SDL_RendererFlip)=dlsym(RTLD_NEXT,"SDL_RenderCopyEx");
 int w=0,h=0;SDL_QueryTexture(t,NULL,NULL,&w,&h);
 if(w==1024 && h==416){++hd;if(d && d->w==512)++zoomed;}
 if(w<100)++dos;
 if(f&SDL_FLIP_HORIZONTAL)++mirrored;
 return real_copy(r,t,s,d,a,c,f);
}
void SDL_RenderPresent(SDL_Renderer *r) {
 void (*real_present)(SDL_Renderer *)=dlsym(RTLD_NEXT,"SDL_RenderPresent");
 ++frames;const char *title=SDL_GetWindowTitle(SDL_RenderGetWindow(r));
 switch(frames) {
 case 10:verify(title,"DOS HERO");break;
 case 18:verify(title,"HD HERO");break;
 case 26:verify(title,"3 fps");break;
 case 34:verify(title,"12 fps");break;
 case 41:verify(title,"PAUSED");break;
 case 44:verify(title,"12 fps");break;
 default:break;
 }
 real_present(r);
 SDL_Keycode key=0;
 switch(frames) {
 case 8:case 16:key=SDLK_TAB;break;
 case 24:case 32:key=SDLK_s;break;
 case 40:case 43:key=SDLK_SPACE;break;
 case 41:key=SDLK_PERIOD;break;
 case 42:key=SDLK_COMMA;break;
 case 50:key=SDLK_RIGHT;break;
 case 60:key=SDLK_LEFT;break;
 case 70:key=SDLK_a;break;
 case 74:case 78:key=SDLK_z;break;
 case 80:key=SDLK_HOME;break;
 default:break;
 }
 if(key){SDL_Event e={0};e.type=SDL_KEYDOWN;e.key.keysym.sym=key;SDL_PushEvent(&e);++events;}
}
void SDL_Quit(void) {
 void (*real_quit)(void)=dlsym(RTLD_NEXT,"SDL_Quit");
 fprintf(stderr,"HD controls: frames=%u events=%u assertions=%u hd=%u dos=%u mirrored=%u zoomed=%u failures=%u\n",frames,events,checks,hd,dos,mirrored,zoomed,failed);
 real_quit();
 if(failed || frames!=96 || events!=14 || checks!=6 || !hd || !dos || !mirrored || !zoomed)exit(2);
}
