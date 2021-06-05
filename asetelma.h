#include <tekstigraf.h>
#include <SDL2/SDL_pixels.h>

#ifndef __asetelmaH__
#define __asetelmaH__
#define Poista_ttuurit(olio) poista_ttuurit(&olio ## ol)
#define SKM(funk) sana=funk(sana); kaan=funk(kaan); meta=funk(meta)
#define SKMARGS(funk, ...) funk(sana, __VA_ARGS__); funk(kaan, __VA_ARGS__); funk(meta, __VA_ARGS__)
#define SKMLIIKU(seur) sana=sana->seur; kaan=kaan->seur; meta=meta->seur;
#define META (*(metatied*)(meta->p))
#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)

typedef struct {
  int parinro;
  int tiednro;
  int kierroksia;
  int osattu;
} metatied;
#endif

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
extern tekstiolio_s annetutol;
extern tekstiolio_s viestiol;
extern tekstiolio_s kysytytol;
extern tekstiolio_s tiedotol;

extern strlista* sana;
extern strlista* kaan;
extern ylista* meta;
extern unsigned osaamisraja;

void asetelma();
void tuhoa_asetelma();
