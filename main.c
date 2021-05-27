#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <strlista.h>
#include "asetelma.h"

SDL_Window *ikkuna;
SDL_Renderer *rend;
char* tmpc;

void kaunnista();

int main(int argc, char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Virhe: Ei voi alustaa SDL-grafiikkaa: %s\n", SDL_GetError());
    return 1;
  }
  if (TTF_Init()) {
    fprintf(stderr, "Virhe: Ei voi alustaa SDL_ttf-fonttikirjastoa: %s\n", \
	    TTF_GetError());
    SDL_Quit();
    return 1;
  }
  ikkuna = SDL_CreateWindow\
    (ohjelman_nimi, ikkuna_x0, ikkuna_y0, ikkuna_w0, ikkuna_h0, SDL_WINDOW_RESIZABLE);
  rend = SDL_CreateRenderer(ikkuna, -1, SDL_RENDERER_TARGETTEXTURE);
  tmpc = malloc(maxpit_suote);
  tmpc[0] = '\0';

  asetelma();
  /*komennot komentoriviltä tähän*/
  char* apuc;
  if(argc > 1)
    strcpy(tmpc, argv[1]);
  for(int i=2; i<argc; i++) {
    apuc = tmpc + strlen(tmpc);
    sprintf(apuc, " %s", argv[i]);
  }

  aseta_vari(taustavari);
  SDL_RenderClear(rend);
  SDL_RenderPresent(rend);
  
  kaunnista();

  free(tmpc);
  tuhoa_asetelma();
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(ikkuna);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
