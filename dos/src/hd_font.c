/* Antialiased outline glyphs; native text metrics remain authoritative.
 * stb_truetype (Sean Barrett, public domain/MIT) is compiled C, no extra libs.
 * Uses bundled, licensed DejaVu font; never reads arbitrary user fonts. */
#define STB_TRUETYPE_IMPLEMENTATION
#include "../tools/vendor/stb_truetype.h"
#include <SDL.h>
#include <stdio.h>
static stbtt_fontinfo font;
static unsigned char*bytes;
SDL_Surface*pop_hd_make_glyph(SDL_Surface*n,int code,const char*path){
 if(code<32||code>131)return NULL;
 int keycap=code>=129;int labelcode=code;
 if(code==127)code=0x00b0;
 if(code==128)code=0x2713;
 if(!bytes){
  FILE*f=fopen(path,"rb");if(!f)return NULL;
  if(fseek(f,0,SEEK_END)){fclose(f);return NULL;}long size=ftell(f);rewind(f);
  if(size<12||size>2000000){fclose(f);return NULL;}
  bytes=malloc(size);if(!bytes){fclose(f);return NULL;}
  if(fread(bytes,1,size,f)!=(size_t)size||!stbtt_InitFont(&font,bytes,stbtt_GetFontOffsetForIndex(bytes,0))){fclose(f);free(bytes);bytes=NULL;return NULL;}fclose(f);
 }
 SDL_Surface*mask=SDL_ConvertSurfaceFormat(n,SDL_PIXELFORMAT_ARGB8888,0);if(!mask)return NULL;
 int l=n->w,t=n->h,r=-1,b=-1;
 for(int y=0;y<n->h;y++)for(int x=0;x<n->w;x++){
  Uint32 p=((Uint32*)((Uint8*)mask->pixels+y*mask->pitch))[x];
  if((p>>24)>128&&(p&0xffffff)){if(x<l)l=x;if(x>r)r=x;if(y<t)t=y;if(y>b)b=y;}
 }SDL_FreeSurface(mask);
 SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,n->w*4,n->h*4,32,SDL_PIXELFORMAT_ARGB8888);if(!out)return NULL;SDL_FillRect(out,NULL,0);
 if(r<l||b<t||code==32)return out;
 if(keycap){
  const char*label=labelcode==129?"ALT":labelcode==130?"CTRL":"ESC";
  int ascent,descent;stbtt_GetFontVMetrics(&font,&ascent,&descent,NULL);
  float scale=(out->h-6.0f)/(ascent-descent);int units=0;
  for(const char*p=label;*p;p++){int a;stbtt_GetCodepointHMetrics(&font,*p,&a,NULL);units+=a;}
  if(units*scale>out->w-6)scale=(out->w-6.0f)/units;
  for(int y=1;y<out->h-1;y++)for(int x=1;x<out->w-1;x++)
   ((Uint32*)((Uint8*)out->pixels+y*out->pitch))[x]=0xffffffff;
  float x=(out->w-units*scale)/2,baseline=(out->h-(ascent-descent)*scale)/2+ascent*scale;
  for(const char*p=label;*p;p++){
   int w,h,xo,yo,advance;unsigned char*a=stbtt_GetCodepointBitmap(&font,scale,scale,*p,&w,&h,&xo,&yo);
   for(int yy=0;yy<h;yy++)for(int xx=0;xx<w;xx++){
    int dx=(int)x+xo+xx,dy=(int)baseline+yo+yy;if(dx>=0&&dx<out->w&&dy>=0&&dy<out->h)
     ((Uint32*)((Uint8*)out->pixels+dy*out->pitch))[dx]=((Uint32)(255-a[yy*w+xx])<<24)|0xffffff;
   }stbtt_FreeBitmap(a,NULL);stbtt_GetCodepointHMetrics(&font,*p,&advance,NULL);x+=advance*scale;
  }return out;
 }
 int x0,y0,x1,y1;if(!stbtt_GetCodepointBox(&font,code,&x0,&y0,&x1,&y1)||x1<=x0||y1<=y0){SDL_FreeSurface(out);return NULL;}
 int w=(r-l+1)*4,h=(b-t+1)*4;
 unsigned char*alpha=calloc((size_t)w*h,1);if(!alpha){SDL_FreeSurface(out);return NULL;}
 stbtt_MakeCodepointBitmap(&font,alpha,w,h,w,(float)(w-1)/(x1-x0),(float)(h-1)/(y1-y0),code);
 for(int y=0;y<h;y++)for(int x=0;x<w;x++)((Uint32*)((Uint8*)out->pixels+(t*4+y)*out->pitch))[l*4+x]=((Uint32)alpha[y*w+x]<<24)|0xffffff;
 free(alpha);return out;
}
void pop_hd_font_shutdown(void){free(bytes);bytes=NULL;}
