/* Regression for offsets introduced by locally fitted column join strips. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static SDL_Surface*load(int id){char p[128];snprintf(p,sizeof(p),"data/hd/VDUNGEON/res%d.png",id);SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s\n",IMG_GetError());exit(1);}SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;}
int main(void){
 const int ids[]={286,287,288,289,291},lo[]={56,8,24,8,24},first[]={0,0,0,80,88},last[]={136,128,136,252,252};
 for(int k=0;k<5;k++){
  SDL_Surface*s=load(ids[k]);
  for(int y=first[k];y<last[k];y++)for(int x=0;x<s->w;x++){
   int opaque=(px(s,x,y)>>24)>128,expected=x>=lo[k]&&x<lo[k]+44;
   if(opaque!=expected){fprintf(stderr,"Column %d profile step at %d,%d\n",ids[k],x,y);return 1;}
  }SDL_FreeSurface(s);
 }
 const int top[]={291,289,291},bottom[]={286,287,288},tx[]={24,8,24},bx[]={56,8,24};
 for(int k=0;k<3;k++){
  SDL_Surface*a=load(top[k]),*b=load(bottom[k]);
  int total=0;
  for(int x=1;x<43;x++)for(int shift=0;shift<=16;shift+=8){
   int p=(px(a,tx[k]+x,a->h-1)>>shift)&255,q=(px(b,bx[k]+x,0)>>shift)&255;
   total+=abs(p-q);
   if(abs(p-q)>32){fprintf(stderr,"Column color discontinuity %d/%d x=%d: %d/%d\n",top[k],bottom[k],x,p,q);return 1;}
  }
  if(total>42*3*8){fprintf(stderr,"Column join has a broad color band\n");return 1;}
  SDL_FreeSurface(a);SDL_FreeSurface(b);
 }
 /* The foreground base must be an exact crop of the background body.
  * Check the whole pedestal, where independent paintings used to overlap. */
 SDL_Surface*base=load(286),*front=load(288),*floor_art=load(241);
 int body_pixels=0,floor_pixels=0;
 for(int y=128;y<240;y++)for(int x=0;x<128;x++){
  Uint32 p=px(base,x,y);int fx=x-32;
  Uint32 f=fx>=0&&fx<front->w?px(front,fx,y):0;
  if((f>>24)==255){
   body_pixels++;
   for(int sh=0;sh<=16;sh+=8)if(abs((int)((p>>sh)&255)-(int)((f>>sh)&255))>1){fprintf(stderr,"Overlapping pedestal images differ at %d,%d\n",x,y);return 1;}
  }else if(!(f>>24)&&y>=188){
   floor_pixels++;
   if(p!=px(floor_art,x,y-188)){fprintf(stderr,"Pedestal platform differs from adjacent floor\n");return 1;}
  }
 }
 if(body_pixels<1000||floor_pixels<100)return 1;
 SDL_FreeSurface(base);SDL_FreeSurface(front);SDL_FreeSurface(floor_art);
 /* A complete rear plinth must retain both sides of its foot instead of
  * tapering to the diagonal edge of the platform. Floor below stays intact. */
 SDL_Surface*rear=load(287),*rear_floor=load(242);int left=104,right=-1;
 for(int x=0;x<104;x++)if(px(rear,x,200)!=px(rear_floor,x,12)){if(x<left)left=x;right=x;}
 if(left>6||right<54||abs(left+right-59)>3){fprintf(stderr,"Rear pedestal foot is clipped or off-center\n");return 1;}
 for(int y=208;y<248;y++)for(int x=0;x<104;x++)if(px(rear,x,y)!=px(rear_floor,x,y-188)){fprintf(stderr,"Rear pedestal leaks into platform side\n");return 1;}
 SDL_FreeSurface(rear);SDL_FreeSurface(rear_floor);
 puts("Rear pedestal: centered complete foot and intact platform PASS");
 puts("Pedestal overlay crop and surrounding floor continuity PASS");
 puts("Column profiles: continuous shaft contours and matching join colors PASS");return 0;
}
