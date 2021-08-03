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
const char* ohjelman_nimi = "csanat";
const char* kotihak = "/home/antterkk";

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

tekstiolio_s suoteol = {.ttflaji = 1,					\
			.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			.fonttikoko = 45,				\
			.vari = (SDL_Color){255,255,255,255},		\
			.sij = {0,0,700,100}};

tekstiolio_s kysymysol = {.ttflaji = 1,					\
			  .vari = (SDL_Color){255,255,255,255},		\
			  .sij = {0,0,700,100}};

tekstiolio_s kauntiol = {.ttflaji = 1,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = {255,255,255,255},			\
			 .sij = {0,0,600,1000},				\
			 .lopusta = 0};
			 
tekstiolio_s tiedotol = {.ttflaji = 1,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = (SDL_Color){255,255,255,255},		\
			 .lopusta = 0,					\
			 .sij = (SDL_Rect){200,0,200,300}};

tekstiolio_s viestiol = {.ttflaji = 1,					\
			 .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			 .fonttikoko = 15,				\
			 .vari = {255,255,255,255},			\
			 .sij = {320,0},				\
			 .lopusta = 1};

lista* snsto;
lista* kysynnat;
int osaamisraja = 1;
int kohdistin = 0;
SDL_Rect kohdistinsij;

void asetelma() {
  snsto = alusta_lista(11*3);
  kysynnat = alusta_lista(11*2);

  suoteol.teksti = calloc(maxpit_suote, 1);
  suoteol.font = TTF_OpenFont(suoteol.fonttied, suoteol.fonttikoko);
  suoteol.sij.y = TTF_FontLineSkip(suoteol.font);

  kysymysol.font = suoteol.font;

  kauntiol.lista = kysynnat;
  kauntiol.font = TTF_OpenFont(kauntiol.fonttied, kauntiol.fonttikoko);
  kauntiol.sij.y = 2*TTF_FontLineSkip(suoteol.font);
  
  viestiol.sij.w = ikkuna_w0 - viestiol.sij.x;
  viestiol.sij.h = ikkuna_h0;
  viestiol.font = TTF_OpenFont(viestiol.fonttied, viestiol.fonttikoko);

  tiedotol.font = TTF_OpenFont(tiedotol.fonttied, tiedotol.fonttikoko);
  tiedotol.lista = alusta_lista(2);
  tiedotol.lista->pit = 2;
  for(int i=0; i<2; i++)
    tiedotol.lista->taul[i] = malloc(24);
  tiedotol.sij.y = kauntiol.sij.y;

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

void tuhoa_asetelma() {
  free(suoteol.teksti);
  TTF_CloseFont(suoteol.font);
  TTF_CloseFont(kauntiol.font);
  TTF_CloseFont(viestiol.font);
  TTF_CloseFont(tiedotol.font);
  if(viestiol.lista)
    viestiol.lista = tuhoa_lista(viestiol.lista);
  tiedotol.lista = tuhoa_lista(tiedotol.lista);
  snsto = tuhoa_lista(snsto);
  kysynnat = tuhoa_lista(kysynnat);
}
