#ifndef LOAD_PNG_H
#define LOAD_PNG_H

#include <SDL/SDL_endian.h>
#include <SDL/SDL_video.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Surface *EXT_LoadPNG_RW(SDL_RWops *src);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _SDL_IMAGE_H */