#ifndef POP_HD_GAME_H
#define POP_HD_GAME_H
#include <SDL.h>
void pop_hd_weapon_record(int index,int group,int resource,int direction);
void pop_hd_weapon_object(int index);
void pop_hd_weapon_mid(int index);
void pop_hd_weapon_draw(int index);
void pop_hd_weapon_end(void);
void pop_hd_shadow_draw(SDL_Surface *image,int pass);
void pop_hd_weapon_bounds(int index,int *dx,int *dy);
void pop_hd_enable(int enabled);
int pop_hd_active(void);
int pop_hd_visible(void);
void pop_hd_toggle(void);
void pop_hd_note(const char *name,int resource);
void pop_hd_bind(SDL_Surface *surface);
int pop_hd_blit(SDL_Surface *source,const SDL_Rect *sr,SDL_Surface *target,SDL_Rect *dr);
int pop_hd_fill(SDL_Surface *target,const SDL_Rect *rect,Uint32 color);
void pop_hd_free(SDL_Surface *surface);
void pop_hd_mirror(SDL_Surface *source,SDL_Surface *target,int vertical);
void pop_hd_mono(SDL_Surface *source,SDL_Surface *target,Uint32 rgb);
void pop_hd_outline(SDL_Surface *target,const SDL_Rect *rect,Uint32 color);
void pop_hd_floor_background(SDL_Surface *surface);
void pop_hd_scene_base(SDL_Surface *surface);
void pop_hd_scene_prepare(SDL_Surface *surface);
void pop_hd_xor(SDL_Surface *target,SDL_Surface *source,const SDL_Rect *rect);
void pop_hd_invalidate(SDL_Surface *surface);
void pop_hd_expand_peel(SDL_Surface *surface);
void pop_hd_fade(SDL_Surface *target,SDL_Surface *source,int amount);
int pop_hd_present(SDL_Renderer *renderer,SDL_Surface *surface);
SDL_Surface *pop_hd_capture(SDL_Surface *surface);
void pop_hd_shutdown(void);
#endif
