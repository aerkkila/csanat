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

int ikkuna_x=0;
int ikkuna_y=0;
int ikkuna_w=0;
int ikkuna_h=0;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2000;

SDL_Color tv = (SDL_Color){0,0,0,255};

tekstiolio_s suoteol;
tekstiolio_s annetutol;

void asetelma() {
  suoteol.teksti = calloc(maxpit_suote, 1);
  suoteol.ttflaji = 2;
  suoteol.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
  suoteol.fonttikoko = 45;
  suoteol.vari = (SDL_Color){255,255,255,255};
  static SDL_Rect suoteolsij = {100,100,500,100}; suoteol.sij = &suoteolsij;
  static SDL_Rect suoteoltot = {0,0,0,0}; suoteol.toteutuma = &suoteoltot;
  suoteol.font = TTF_OpenFont(suoteol.fonttied, suoteol.fonttikoko);

  annetutol.lista = _yuusi_lista();
  annetutol.ttflaji = 2;
  annetutol.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
  annetutol.fonttikoko = 15;
  annetutol.vari = (SDL_Color){255,255,255,255};
  static SDL_Rect annetutolsij = {0,0,1000,1000}; annetutol.sij = &annetutolsij;
  static SDL_Rect annetutoltot = {0,0,0,0}; annetutol.toteutuma = &annetutoltot;
  annetutol.lopusta = 0;
  annetutol.font = TTF_OpenFont(annetutol.fonttied, annetutol.fonttikoko);
}

void tuhoa_asetelma() {
  free(suoteol.teksti);
  TTF_CloseFont(suoteol.font);
  _strpoista_kaikki(_yalkuun(annetutol.lista));
  TTF_CloseFont(annetutol.font);
}
