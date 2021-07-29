#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include "lista.h"
#include "grafiikka.h"
#include "asetelma.h"

const int ikkuna_x0 = 150;
const int ikkuna_y0 = 200;
const int ikkuna_w0 = 600;
const int ikkuna_h0 = 400;
const char ohjelman_nimi[] = "csanat";

int ikkuna_x;
int ikkuna_y;
int ikkuna_w;
int ikkuna_h;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2000;

SDL_Color taustavari = (SDL_Color){0,0,0,255};
SDL_Color oikeavari = (SDL_Color){20,255,20,255};
SDL_Color virhevari = (SDL_Color){255,20,20,255};
SDL_Color apuvari;
int suoteviesti = 0;

const char kotihak[] = "/home/antterkk";

tekstiolio_s suoteol = {.ttflaji = 2,					\
			.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			.fonttikoko = 45,				\
			.vari = (SDL_Color){255,255,255,255},		\
			.sij = {0,200,700,100}};

tekstiolio_s kysymysol = {.ttflaji = 1,					\
			  .vari = (SDL_Color){255,255,255,255},		\
			  .sij = {0,0,700,100}};

tekstiolio_s kauntiol = {.ttflaji = 2,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = {255,255,255,255},			\
			 .sij = {0,230,1000,1000},			\
			 .lopusta = 0};

tekstiolio_s viestiol = {.ttflaji = 2,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = {255,255,255,255},			\
			 .sij = {320,0},				\
			 .lopusta = 1};
			 
tekstiolio_s tiedotol = {.ttflaji = 2,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = (SDL_Color){255,255,255,255},		\
			 .lopusta = 0,					\
			 .sij = (SDL_Rect){200,0,200,300}};

lista* snsto;
lista* kysynnat;
unsigned osaamisraja = 1;
SDL_Texture* alusta;
SDL_Texture* apualusta;

void asetelma() {
  extern SDL_Renderer* rend;
  snsto = alusta_lista(16*3);
  kysynnat = alusta_lista(16*2);

  suoteol.teksti = calloc(maxpit_suote, 1);
  suoteol.font = TTF_OpenFont(suoteol.fonttied, suoteol.fonttikoko);

  kysymysol.font = suoteol.font;

  kauntiol.lista = kysynnat;
  kauntiol.font = TTF_OpenFont(kauntiol.fonttied, kauntiol.fonttikoko);
  
  viestiol.sij.w = ikkuna_w0 - viestiol.sij.x;
  viestiol.sij.h = ikkuna_h0;
  viestiol.font = TTF_OpenFont(viestiol.fonttied, viestiol.fonttikoko);
  viestiol.lista = alusta_lista(16);

  tiedotol.font = TTF_OpenFont(tiedotol.fonttied, tiedotol.fonttikoko);

  ikkuna_x = ikkuna_x0;
  ikkuna_y = ikkuna_y0;
  ikkuna_w = ikkuna_w0;
  ikkuna_h = ikkuna_h0;
  
  alusta = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikkuna_w0, ikkuna_h0);
  apualusta = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikkuna_w0, ikkuna_h0);
}

void tuhoa_asetelma() {
  free(suoteol.teksti);
  TTF_CloseFont(suoteol.font);
  TTF_CloseFont(kauntiol.font);
  TTF_CloseFont(viestiol.font);
  TTF_CloseFont(tiedotol.font);
  viestiol.lista = tuhoa_lista(viestiol.lista);
  snsto = tuhoa_lista(snsto);
  kysynnat = tuhoa_lista(kysynnat);
  SDL_DestroyTexture(alusta);
  SDL_DestroyTexture(apualusta);
}
