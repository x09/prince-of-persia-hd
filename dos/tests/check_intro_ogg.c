/* Verify the actual game loader selects and decodes the replacement. */
#define POP_HD_INTERNAL
#include "common.h"
int main(int argc,char **argv) {
 g_argc=argc;g_argv=argv;
 if(SDL_Init(SDL_INIT_AUDIO))return 1;
 load_sound_names();
 sound_buffer_type *s=load_sound(sound_54_intro_music);
 if(!s || s->type!=sound_ogg) {
  fprintf(stderr,"Intro did not load as OGG\n");return 2;
 }
 short pcm[4096];unsigned long frames=0,nonzero=0;int n;
 while((n=stb_vorbis_get_samples_short_interleaved(s->ogg.decoder,2,pcm,4096))>0) {
  frames+=n;
  for(int i=0;i<n*2;i++)if(pcm[i])nonzero++;
 }
 printf("Game intro OGG: frames=%lu rate=44100 duration=%.3f nonzero=%lu\n",
 frames,frames/44100.0,nonzero);
 free_sound(s);SDL_CloseAudio();SDL_Quit();
 return frames>1000000 && frames<1200000 && nonzero>100000?0:3;
}
