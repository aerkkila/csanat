#ifndef __asetelma_h__
#define __asetelma_h__

#include <SDL2/SDL_pixels.h>
#include "grafiikka.h"

#define META_KIERROKSIA 2
#define META_OSAAMISIA 3
#define META_OSAAMISET 4

#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)

extern const int ikkuna_x0;
extern const int ikkuna_y0;
extern const int ikkuna_w0;
extern const int ikkuna_h0;
extern const char ohjelman_nimi[];

extern int ikkuna_x;
extern int ikkuna_y;
extern int ikkuna_w;
extern int ikkuna_h;

extern SDL_Color taustavari;
extern SDL_Color oikeavari;
extern SDL_Color virhevari;
extern SDL_Color apuvari;
extern int suoteviesti;

extern const unsigned uniaika;
extern const unsigned maxpit_suote;

extern const char kotihak[];

extern tekstiolio_s suoteol;
extern tekstiolio_s kysymysol;
extern tekstiolio_s kauntiol; //sisältää annetut ja kysytyt
extern tekstiolio_s viestiol; //käytetään esim ls:n tulokseen
extern tekstiolio_s tiedotol;

extern lista* snsto;
extern lista* kysynnat;
extern unsigned osaamisraja;

extern SDL_Texture* alusta;
extern SDL_Texture* apualusta;

void asetelma();
void tuhoa_asetelma();
#endif
