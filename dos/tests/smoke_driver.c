/* Runtime-only SDL observer: no game state writes or gameplay replacement. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <SDL.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static void (*original_callback)(void *,Uint8 *,int);
static void *original_userdata;
static SDL_AudioFormat sample_format;
static uint64_t samples,nonzero;
static unsigned peak,frames;
static Uint32 start;
static int stopping;
static void observe_audio(void *unused,Uint8 *stream,int length) {
 (void)unused;original_callback(original_userdata,stream,length);
 if(sample_format==AUDIO_S16SYS) {
  const int16_t *pcm=(const int16_t *)stream;
  for(int i=0;i<length/2;++i) {
   int v=pcm[i];unsigned a=(unsigned)(v<0?-v:v);
   ++samples;if(a)++nonzero;if(a>peak)peak=a;
  }
 }
}
int SDL_OpenAudio(SDL_AudioSpec *wanted,SDL_AudioSpec *obtained) {
 int (*real_open)(SDL_AudioSpec *,SDL_AudioSpec *)=dlsym(RTLD_NEXT,"SDL_OpenAudio");
 SDL_AudioSpec spec=*wanted;
 original_callback=spec.callback;original_userdata=spec.userdata;
 sample_format=spec.format;spec.callback=observe_audio;spec.userdata=NULL;
 int result=real_open(&spec,obtained);
 fprintf(stderr,"SMOKE audio driver=%s format=%u rate=%d result=%d\n",
  SDL_GetCurrentAudioDriver(),sample_format,spec.freq,result);
 return result;
}
void SDL_RenderPresent(SDL_Renderer *renderer) {
 void (*real_present)(SDL_Renderer *)=dlsym(RTLD_NEXT,"SDL_RenderPresent");
 real_present(renderer);
 if(!frames++) {
  start=SDL_GetTicks();int w=0,h=0;SDL_GetRendererOutputSize(renderer,&w,&h);
  fprintf(stderr,"SMOKE renderer=%s output=%dx%d\n",SDL_GetCurrentVideoDriver(),w,h);
 }
 if(getenv("POP_SMOKE_INPUT")) {
  static unsigned next;
  static const struct {unsigned ms;SDL_Scancode scan;SDL_Keycode key;int down;} script[]={
   {900,SDL_SCANCODE_RIGHT,SDLK_RIGHT,1},{2400,SDL_SCANCODE_UP,SDLK_UP,1},
   {2650,SDL_SCANCODE_UP,SDLK_UP,0},{4100,SDL_SCANCODE_RIGHT,SDLK_RIGHT,0},
   {4500,SDL_SCANCODE_LEFT,SDLK_LEFT,1},{6000,SDL_SCANCODE_LEFT,SDLK_LEFT,0},
   {6200,SDL_SCANCODE_F6,SDLK_F6,1},{6250,SDL_SCANCODE_F6,SDLK_F6,0},
   {7000,SDL_SCANCODE_F6,SDLK_F6,1},{7050,SDL_SCANCODE_F6,SDLK_F6,0},
   {7600,SDL_SCANCODE_LSHIFT,SDLK_LSHIFT,1},{8100,SDL_SCANCODE_LSHIFT,SDLK_LSHIFT,0}
  };
  while(next<sizeof(script)/sizeof(script[0]) && SDL_GetTicks()-start>=script[next].ms) {
   SDL_Event e={0};e.type=script[next].down?SDL_KEYDOWN:SDL_KEYUP;
   e.key.state=script[next].down?SDL_PRESSED:SDL_RELEASED;
   e.key.keysym.scancode=script[next].scan;e.key.keysym.sym=script[next].key;
   if(getenv("POP_SMOKE_LEFT_FIRST")) {
    if(e.key.keysym.scancode==SDL_SCANCODE_RIGHT){e.key.keysym.scancode=SDL_SCANCODE_LEFT;e.key.keysym.sym=SDLK_LEFT;}
    else if(e.key.keysym.scancode==SDL_SCANCODE_LEFT){e.key.keysym.scancode=SDL_SCANCODE_RIGHT;e.key.keysym.sym=SDLK_RIGHT;}
   }
   SDL_PushEvent(&e);fprintf(stderr,"SMOKE input=%u key=%d down=%d\n",next,e.key.keysym.scancode,script[next].down);++next;
  }
 }
 if(getenv("POP_SMOKE_CAPTURE")) {
  static unsigned next_capture=1;
  if(next_capture<=4 && SDL_GetTicks()-start>=next_capture*(getenv("POP_SMOKE_CAPTURE_STEP_MS")?(unsigned)strtoul(getenv("POP_SMOKE_CAPTURE_STEP_MS"),NULL,10):2000)) {
   int w=0,h=0;SDL_GetRendererOutputSize(renderer,&w,&h);
   SDL_Surface *capture=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
   int (*save_png)(SDL_Surface*,const char*)=dlsym(RTLD_DEFAULT,"IMG_SavePNG");
   const char *prefix=getenv("POP_SMOKE_CAPTURE_PREFIX");
   char path[256];snprintf(path,sizeof(path),"%s-%u.png",prefix?prefix:"build/overlap-live",next_capture);
   if(capture && save_png && !SDL_RenderReadPixels(renderer,NULL,capture->format->format,capture->pixels,capture->pitch))save_png(capture,path);
   SDL_FreeSurface(capture);++next_capture;
  }
 }
 unsigned duration=15000;const char *value=getenv("POP_SMOKE_MS");
 if(value)duration=(unsigned)strtoul(value,NULL,10);
 if(!stopping && SDL_GetTicks()-start>=duration) {
  stopping=1;SDL_LockAudio();
  fprintf(stderr,"SMOKE frames=%u elapsed=%u samples=%llu nonzero=%llu peak=%u\n",
   frames,SDL_GetTicks()-start,(unsigned long long)samples,(unsigned long long)nonzero,peak);
  SDL_UnlockAudio();
  SDL_Event event={0};event.type=SDL_QUIT;SDL_PushEvent(&event);
 }
}
void SDL_Quit(void) {
 void (*real_quit)(void)=dlsym(RTLD_NEXT,"SDL_Quit");
 SDL_LockAudio();
 fprintf(stderr,"SMOKE final frames=%u samples=%llu nonzero=%llu peak=%u\n",
  frames,(unsigned long long)samples,(unsigned long long)nonzero,peak);
 SDL_UnlockAudio();real_quit();
}
