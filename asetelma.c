#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include "lista.h"
#include "grafiikka.h"
#include "asetelma.h"
#include "csanat.h"

const int ikkuna_x0 = 150;
const int ikkuna_y0 = 200;
const int ikkuna_w0 = 600;
const int ikkuna_h0 = 400;
const char* ohjelman_nimi = "csanat";
const char* aloituskomentotiedosto = "aloituskomennot.txt";

int ikkuna_x;
int ikkuna_y;
int ikkuna_w;
int ikkuna_h;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2000;

const SDL_Color taustavari       = {0,  0,  0,  255};
const SDL_Color oikeavari        = {20, 255,20, 255};
const SDL_Color virhevari        = {255,20, 20, 255};
const SDL_Color oikea_taustavari = {20, 255,60, 255};
const SDL_Color virhe_taustavari = {255,80, 50, 255};
const SDL_Color kohdistinvari    = {255,255,255,255};
SDL_Color tekstin_taustavari;
SDL_Color apuvari;
int suoteviesti = 0;

static const char* paafontti = "MAKE_LIITÄ_SERIFFONTTI";

tekstiolio_s suoteol = {			\
  .ttflaji = 1,					\
  .fonttikoko = 60,				\
  .vari = (SDL_Color){255,255,255,255},		\
  .sij = {0,0,700,100}				\
};

tekstiolio_s kysymysol = {		    \
  .ttflaji = 1,				    \
  .vari = (SDL_Color){255,255,255,255},	    \
  .sij = {0,0,700,100}			    \
};

tekstiolio_s kauntiol = {			\
  .ttflaji = 1,					\
  .fonttikoko = 20,				\
  .vari = {255,255,255,255},			\
  .sij = {0,0,600,1000},			\
  .lopusta = 0             			\
};

tekstiolio_s tiedotol = {						\
  .ttflaji = 1,								\
  .fonttikoko = 20,							\
  .vari = (SDL_Color){255,255,255,255},					\
  .lopusta = 0,								\
  .sij = (SDL_Rect){200,0,200,300}					\
};

tekstiolio_s viestiol = {						\
  .ttflaji = 1,								\
  .fonttikoko = 17,							\
  .vari = {255,255,255,255},						\
  .sij = {320,0},							\
  .lopusta = 1								\
};

lista* snsto;
lista* kysynnat;
lista* tiedostot;
int osaamisraja = 1;
int kohdistin = 0;
SDL_Rect kohdistinsij;
int alussa = 0;
int edellinen_sij = -1;

static void avaa_fontti(tekstiolio_s* olio) {
  if(!olio->fonttied)
    olio->fonttied = paafontti;
  olio->font = TTF_OpenFont(olio->fonttied, olio->fonttikoko);
  if(!olio->font) {
    fprintf(stderr, "Ei avattu fonttia \"%s\"\n%s\n", olio->fonttied, TTF_GetError());
    exit(1);
  }
}

void asetelma() {
  snsto = alusta_lista(11,snsto_s);
  kysynnat = alusta_lista(11,kysynta_s);
  tiedostot = alusta_lista(1,char**);

  suoteol.teksti = calloc(maxpit_suote, 1);
  avaa_fontti(&suoteol);
  suoteol.sij.y = TTF_FontLineSkip(suoteol.font);

  kysymysol.font = suoteol.font;

  kauntiol.lista = kysynnat;
  avaa_fontti(&kauntiol);
  kauntiol.sij.y = 2*TTF_FontLineSkip(suoteol.font);
  
  viestiol.sij.w = ikkuna_w0 - viestiol.sij.x;
  viestiol.sij.h = ikkuna_h0;
  avaa_fontti(&viestiol);

#define RIVEJA 3
  avaa_fontti(&tiedotol);
  tiedotol.lista = alusta_lista(RIVEJA,char**);
  jatka_listaa(tiedotol.lista, RIVEJA);
  for(int i=0; i<RIVEJA; i++)
    *LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,i) = malloc(32);
  tiedotol.sij.y = kauntiol.sij.y;
#undef RIVEJA

  ikkuna_x = ikkuna_x0;
  ikkuna_y = ikkuna_y0;
  ikkuna_w = ikkuna_w0;
  ikkuna_h = ikkuna_h0;
  tekstin_taustavari = taustavari;

  kohdistinsij.y = suoteol.sij.y;
  kohdistinsij.h = TTF_FontLineSkip(suoteol.font);
  TTF_GlyphMetrics(suoteol.font, ' ', NULL, NULL, NULL, NULL, &kohdistinsij.w);
  kohdistinsij.w /= 10;
  if(!kohdistinsij.w)
    kohdistinsij.w = 1;
}
