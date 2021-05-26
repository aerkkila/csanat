#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <tekstigraf.h>
#include "asetelma.h"

const int ikkuna_x0 = 0;
const int ikkuna_y0 = 0;
const int ikkuna_w0 = 600;
const int ikkuna_h0 = 400;
const char ohjelman_nimi[] = "csanat";

int ikkuna_x;
int ikkuna_y;
int ikkuna_w;
int ikkuna_h;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2000;

SDL_Color tv = (SDL_Color){0,0,0,255};

const char kotihak[] = "/home/antterkk";

tekstiolio_s suoteol = {.ttflaji = 2,					\
			.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			.fonttikoko = 45,				\
			.vari = (SDL_Color){255,255,255,255}};
tekstiolio_s kysymysol = {.ttflaji = 1,					\
			  .vari = (SDL_Color){255,255,255,255}};
tekstiolio_s annetutol;
tekstiolio_s viestiol;
tekstiolio_s kysyntaol = {.ttflaji = 2,					\
			  .fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf", \
			  .fonttikoko = 15,				\
			  .vari = (SDL_Color){255,255,255,255},		\
			  .lopusta = 0};

void asetelma() {
  suoteol.teksti = calloc(maxpit_suote, 1);
  static SDL_Rect suoteolsij = {0,100,700,100}; suoteol.sij = &suoteolsij;
  static SDL_Rect suoteoltot = {0,0,0,0}; suoteol.toteutuma = &suoteoltot;
  suoteol.font = TTF_OpenFont(suoteol.fonttied, suoteol.fonttikoko);

  kysymysol.font = suoteol.font;
  static SDL_Rect kysymysolsij = {0,0,700,100}; kysymysol.sij = &kysymysolsij;
  static SDL_Rect kysymysoltot = {0,0,0,0}; kysymysol.toteutuma = &kysymysoltot;

  annetutol.lista = _yuusi_lista();
  annetutol.ttflaji = 2;
  annetutol.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
  annetutol.fonttikoko = 15;
  annetutol.vari = (SDL_Color){255,255,255,255};
  static SDL_Rect annetutolsij = {0,0,1000,1000}; annetutol.sij = &annetutolsij;
  static SDL_Rect annetutoltot = {0,0,0,0}; annetutol.toteutuma = &annetutoltot;
  annetutol.lopusta = 0;
  annetutol.font = TTF_OpenFont(annetutol.fonttied, annetutol.fonttikoko);

  static SDL_Rect kysyntaolsij = {0,100,500,1000};
  static SDL_Rect kysyntaoltot = {0,0,0,0};
  kysyntaol.sij = &kysyntaolsij;
  kysyntaol.toteutuma = &kysyntaoltot;
  kysyntaol.font = TTF_OpenFont(kysyntaol.fonttied, kysyntaol.fonttikoko);
  
  viestiol.lista = NULL;
  viestiol.ttflaji = 2;
  viestiol.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
  viestiol.fonttikoko = 15;
  viestiol.vari = (SDL_Color){255,255,255,255};
  static SDL_Rect viestiolsij = (SDL_Rect){0,0};
  viestiolsij.w = ikkuna_w0;
  viestiolsij.h = suoteol.sij->y;
  viestiol.sij = &viestiolsij;
  static SDL_Rect viestioltot = {0,0,0,0}; viestiol.toteutuma = &viestioltot;
  viestiol.lopusta = 1;
  viestiol.font = TTF_OpenFont(viestiol.fonttied, viestiol.fonttikoko);

  ikkuna_x = ikkuna_x0;
  ikkuna_y = ikkuna_y0;
  ikkuna_w = ikkuna_w0;
  ikkuna_h = ikkuna_h0;
}

void tuhoa_asetelma() {
  free(suoteol.teksti);
  TTF_CloseFont(suoteol.font);
  _strpoista_kaikki(_yalkuun(annetutol.lista));
  TTF_CloseFont(annetutol.font);
  _strpoista_kaikki(_yalkuun(viestiol.lista));
  TTF_CloseFont(viestiol.font);
  TTF_CloseFont(kysyntaol.font);
  Poista_ttuurit(suote);
  Poista_ttuurit(annetut);
  Poista_ttuurit(viesti);
  Poista_ttuurit(kysynta);
  Poista_ttuurit(kysymys);
}
