/* Offline export using the game's original MIDI parser and AdLib renderer.
 * Link all engine objects except main.o; does not change game playback.
 */
#define POP_HD_INTERNAL
#include "common.h"
#include <stddef.h>
extern short midi_playing;
void free_midi_resources(void);
extern SDL_AudioSpec *digi_audiospec;
/* Keep the opened SDL device paused: rendering is synchronous here. */
void SDL_PauseAudio(int pause_on) {(void)pause_on;}
static void le(FILE *f,unsigned n,int bytes) {
 for(int i=0;i<bytes;i++)fputc((n>>(8*i))&255,f);
}
int main(int argc,char **argv) {
 g_argc=argc;g_argv=argv;
 if(SDL_Init(SDL_INIT_AUDIO))return 1;
 dat_type *dat=open_dat("MIDISND2.DAT",0);
 if(!dat)return 2;
 int size=0;
 sound_buffer_type *b=load_from_opendats_alloc(10054,"bin",NULL,&size);
 close_dat(dat);
 if(!b || b->type!=sound_midi || size<20)return 3;
 size_t offset=offsetof(sound_buffer_type,midi);
 if(memcmp((byte*)b+offset,"MThd",4))return 4;
 FILE *mid=fopen("exports/intro_theme.mid","wb");
 if(!mid)return 5;
 int ok=fwrite((byte*)b+offset,1,size-offset,mid)==(size_t)size-offset;
 if(fclose(mid)||!ok)return 6;
 current_sound=sound_54_intro_music;
 play_midi_sound(b);
 if(!midi_playing || !digi_audiospec || digi_audiospec->format!=AUDIO_S16LSB)return 7;
 FILE *raw=fopen("exports/intro_theme.pcm","wb");
 if(!raw)return 8;
 unsigned char samples[4096];
 unsigned bytes=0;unsigned long long energy=0;
 while(midi_playing && bytes < 180U*digi_audiospec->freq*digi_audiospec->channels*2) {
  memset(samples,0,sizeof(samples));
  midi_callback(NULL,samples,sizeof(samples));
  if(fwrite(samples,1,sizeof(samples),raw)!=sizeof(samples))return 9;
  bytes+=sizeof(samples);
  for(unsigned i=0;i<sizeof(samples);i+=2) {
   short v=(short)(samples[i]|samples[i+1]<<8);
   energy+=(unsigned)(v<0?-v:v);
  }
 }
 fclose(raw);
 if(midi_playing || !energy)return 10;
 FILE *wav=fopen("exports/intro_theme.wav","wb");
 raw=fopen("exports/intro_theme.pcm","rb");
 if(!wav||!raw)return 11;
 fwrite("RIFF",1,4,wav);le(wav,bytes+36,4);fwrite("WAVEfmt ",1,8,wav);
 le(wav,16,4);le(wav,1,2);le(wav,digi_audiospec->channels,2);
 le(wav,digi_audiospec->freq,4);le(wav,digi_audiospec->freq*digi_audiospec->channels*2,4);
 le(wav,digi_audiospec->channels*2,2);le(wav,16,2);
 fwrite("data",1,4,wav);le(wav,bytes,4);
 size_t n;while((n=fread(samples,1,sizeof(samples),raw)))
  if(fwrite(samples,1,n,wav)!=n)return 12;
 int failed=ferror(raw);fclose(raw);failed|=fclose(wav)!=0;
 printf("Intro export: MIDI=%d bytes, PCM=%u bytes, rate=%d channels=%d duration=%.3f s energy=%llu\n",
 size-(int)offset,bytes,digi_audiospec->freq,digi_audiospec->channels,
 (double)bytes/(digi_audiospec->freq*digi_audiospec->channels*2),energy);
 free(b);free_midi_resources();SDL_CloseAudio();SDL_Quit();
 return failed?13:0;
}
