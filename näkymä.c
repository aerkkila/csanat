#include "näkymä.h"
#include <stdlib.h>
#include <stdio.h>

SDL_Window* ikk;
SDL_Renderer* rend;
SDL_Texture* pohja;

int ikk_x=300, ikk_y=0, ikk_w=600, ikk_h=400;
const SDL_Color etuvari          = {255,255,255,255};
const SDL_Color taustavari       = {0,  0,  0,  0  };
const SDL_Color oikeavari        = {20, 255,20, 255};
const SDL_Color virhevari        = {255,20, 20, 255};
const SDL_Color oikea_taustavari = {20, 255,60, 255};
const SDL_Color virhe_taustavari = {255,80, 50, 255};
const SDL_Color kohdistinvari    = {255,255,255,255};
nakyolio syoteol = { .fkoko=60, .etuvari=&etuv, .taustav=&taustavari };
nakyolio kysymol = { .fkoko=60, .etuvari=&etuv, .taustav=&taustavari };
nakyolio listaol = { .fkoko=20, .etuvari=&etuv, .taustav=&taustavari };
nakyolio tietool = { .fkoko=17, .etuvari=&etuv, .taustav=&taustavari };

static const char fonttied = "MAKE_LIITÄ_MONOFONTTI";

void avaa_fontti(nakyolio* olio) {
  olio->font = TTF_OpenFont( fonttied, olio->fkoko );
  if(!olio->font) {
    fprintf( stderr, "Ei avattu fonttia \"%s\"\n%s\n", olio->fnimi, TTF_GetError() );
    exit(EXIT_FAILURE);
  }
}

void paivita_sijainnit() {
  syoteol.sij.y = TTF_FontLineSkip(kysymol.font);
  listaol.sij.y = syoteol.sij.y + TTF_FontLineSkip(syoteol.font);
  tietool.sij.y = listaol.sij.y;
}

#define FUNK(fun,virhefun,...)						\
  do {									\
  if(fun(__VA_ARGS__))							\
    {									\
      fprintf( stderr, "Virhe funktiossa " #fun ": %s\n", virhefun() ); \
      exit(EXIT_FAILURE);						\
    }									\
  while(0)

void alusta_nakyma() {
  FUNK(SDL_Init,SDL_GetError,SDL_INIT_VIDEO);
  FUNK(TTF_Init,TTF_GetError);
  ikk = SDL_CreateWindow( ohjelan_nimi, ikk_x, ikk_y, ikk_w, ikk_h, SDL_WINDOW_RESIZABLE );
  rend = SDL_CreateRenderer( ikk, -1, SDL_RENDERER_TARGETTEXTURE );
  pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikk_w, ikk_h );

  avaa_fontti(&syoteol);
  avaa_fontti(&kysymol);
  avaa_fontti(&listaol);
  avaa_fontti(&tietool);

  SDL_GetWindowSize( ikk, &ikk_w, &ikk_h );
  paivita_sijainnit();
}
