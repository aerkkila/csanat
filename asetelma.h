#ifndef __asetelma_h__
#define __asetelma_h__

#include <SDL2/SDL_pixels.h>
#include "grafiikka.h"

extern const int ikkuna_x0;
extern const int ikkuna_y0;
extern const int ikkuna_w0;
extern const int ikkuna_h0;
extern const char* ohjelman_nimi;
extern const char* kotihak;
extern const char* aloituskomentotiedosto;

extern int ikkuna_x;
extern int ikkuna_y;
extern int ikkuna_w;
extern int ikkuna_h;

extern const SDL_Color taustavari;
extern const SDL_Color oikeavari;
extern const SDL_Color virhevari;
extern const SDL_Color oikea_taustavari;
extern const SDL_Color virhe_taustavari;
extern const SDL_Color kohdistinvari;
extern SDL_Color tekstin_taustavari;
extern SDL_Color apuvari;
extern int suoteviesti;

extern const unsigned uniaika;
extern const unsigned maxpit_suote;

extern tekstiolio_s suoteol;
extern tekstiolio_s kysymysol;
extern tekstiolio_s kauntiol; //sisältää annetut ja kysytyt
extern tekstiolio_s viestiol; //käytetään esim ls:n tulokseen
extern tekstiolio_s tiedotol;

extern lista* snsto;
extern lista* kysynnat;
extern lista* tiedostot;
extern int osaamisraja;
extern int kohdistin; //kasvaa oikealta vasemmalle, mihin väliin kirjain laitetaan
extern SDL_Rect kohdistinsij;
extern int alussa;
extern int edellinen_sij;

void asetelma();
void tuhoa_asetelma();
#endif
