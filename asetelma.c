#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <tekstigraf.h>
#include "asetelma.h"

const int ikkuna_x0 = 0;
const int ikkuna_y0 = 0;
const int ikkuna_w0 = 500;
const int ikkuna_h0 = 200;
const char ohjelman_nimi[] = "csanat";

int ikkuna_x=0;
int ikkuna_y=0;
int ikkuna_w=0;
int ikkuna_h=0;

const unsigned uniaika = 10;
const unsigned maxpit_suote = 2000;

tekstiolio_s suoteol;

void asetelma() {
  suoteol.teksti = calloc(maxpit_suote, 1);
  suoteol.ttflaji = 2;
  suoteol.fonttied = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";
  suoteol.fonttikoko = 45;
  suoteol.vari = (SDL_Color){255,255,255,255};
  static SDL_Rect suoteolsij = {100,100,400,100}; suoteol.sij = &suoteolsij;
  static SDL_Rect suoteoltot = {0,0,0,0}; suoteol.toteutuma = &suoteoltot;

  suoteol.font = TTF_OpenFont(suoteol.fonttied, suoteol.fonttikoko);
}

void tuhoa_asetelma() {
  free(suoteol.teksti);
}
