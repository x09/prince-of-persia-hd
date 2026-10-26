/* Assemble input streams; only the first replay's savestate and RNG seed survive.
 * Later per-level savestates are discarded. A merged movie is NOT a passing
 * campaign until the unmodified engine actually reaches its win room. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef struct { unsigned char *data; size_t size, count_pos, moves; uint32_t count; } Movie;
static uint32_t le32(const unsigned char *p) {
 return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void fail(const char *s) { perror(s); exit(2); }
static Movie read_movie(const char *path) {
 Movie m={0}; FILE *f=fopen(path,"rb"); if(!f)fail(path);
 fseek(f,0,SEEK_END); long size=ftell(f); rewind(f);
 if(size<32 || size>10000000)fail("movie size");
 m.size=(size_t)size; m.data=malloc(m.size); if(!m.data)fail("malloc");
 if(fread(m.data,1,m.size,f)!=m.size)fail("read");
 fclose(f);
 if(memcmp(m.data,"P1R",3) || m.data[5]!=102)fail("P1R version");
 size_t p=15;
 for(int i=0;i<2;i++){if(p>=m.size)fail("string"); size_t n=m.data[p++]; p+=n;}
 for(int i=0;i<6;i++){if(p+4>m.size)fail("section"); uint32_t n=le32(m.data+p);p+=4+n;}
 p+=2+4; if(p+4>m.size)fail("count");
 m.count_pos=p; m.count=le32(m.data+p); m.moves=p+4;
 if(m.moves+m.count!=m.size)fail("move count");
 return m;
}
int main(int argc,char **argv) {
 if(argc!=3 && argc!=4){fprintf(stderr,"usage: %s output.p1r waits.txt\n",argv[0]);return 2;}
 int limit=argc==4?atoi(argv[3]):14;if(limit<1||limit>14)return 2;
 unsigned waits[15]={0}; FILE *w=fopen(argv[2],"r"); if(!w)fail(argv[2]);
 int level; unsigned delay;
 while(fscanf(w,"%d %u",&level,&delay)==2){
  if(level<1||level>14||delay>10000)fail("wait range");
  waits[level]=delay;
 } fclose(w);
 Movie movies[15]={{0}}; uint32_t count=0;
 for(int n=1;n<=limit;n++){
  char path[128];snprintf(path,sizeof(path),"tests/campaign/level%02d.p1r",n);
  movies[n]=read_movie(path);count+=movies[n].count+waits[n];
 }
 const char *seed_text=getenv("POP_FULL_SEED");
 if(seed_text){uint32_t seed=(uint32_t)strtoul(seed_text,NULL,0);unsigned char*p=movies[1].data+movies[1].count_pos-4;for(int k=0;k<4;k++)p[k]=(unsigned char)(seed>>(8*k));}
 FILE *out=fopen(argv[1],"wb");if(!out)fail(argv[1]);
 fwrite(movies[1].data,1,movies[1].count_pos,out);
 unsigned char count_bytes[4]={count,count>>8,count>>16,count>>24};
 fwrite(count_bytes,1,4,out);
 unsigned tick=0;
 for(int n=1;n<=limit;n++){
  unsigned split=n==7?19:7; if(split>movies[n].count)split=movies[n].count;
  fprintf(stderr,"MERGE level=%d tick=%u moves=%u wait=%u split=%u\n",
    n,tick,movies[n].count,waits[n],split);
  fwrite(movies[n].data+movies[n].moves,1,split,out);
  for(unsigned k=0;k<waits[n];k++)fputc(0,out);
  fwrite(movies[n].data+movies[n].moves+split,1,movies[n].count-split,out);
  tick+=movies[n].count+waits[n];free(movies[n].data);
 }
 if(fclose(out))fail("close");
 return 0;
}
