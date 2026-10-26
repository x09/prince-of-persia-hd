#ifndef POP_HD_PERSIALITY_H
#define POP_HD_PERSIALITY_H
#include <SDL.h>
int pop_persiality_active(void);
int pop_persiality_key(SDL_Scancode code, SDL_Keymod mod, int repeat);
int pop_persiality_step(void);
void pop_persiality_draw(SDL_Renderer *renderer);
void pop_persiality_shutdown(void);
#endif
