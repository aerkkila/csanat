#ifndef __TEKSTIGRAFIIKKA__
#define __TEKSTIGRAFIIKKA__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "lista.h"

typedef struct {
  char* teksti;
  lista* lista;
  char ttflaji; //mitä ttf-kirjaston funktiota käytetään (katso laita_teksti_ttf())
  TTF_Font* font;
  int fonttikoko;
  char* fonttied;
  SDL_Rect sij; //tälle varattu maksimitila
  SDL_Rect toteutuma; //mikä tila oikeasti käytetään
  SDL_Color vari;
  short alku; //koskee vain listoja, ensimmäisen näytetyn indeksi, 'r'
  short rullaus; //koskee vain listoja, 'w'
  char lopusta; //laitetaanko lista lopusta vai alusta
  char numerointi; //koskee vain listoja;
  SDL_Texture *ttuuri; //osoitin olkoon listan lopussa
} tekstiolio_s;

void laita_teksti_ttf(tekstiolio_s*);
int laita_tekstilista(lista*, int, tekstiolio_s*);
void laita_kaunti();

#endif