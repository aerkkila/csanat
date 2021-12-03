#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "asetelma.h"
#include "menetelmiä.h"

SDL_Window *ikkuna;
SDL_Renderer *rend;
SDL_Texture* tausta;
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
  tausta = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikkuna_w, ikkuna_h);
  tmpc = malloc(maxpit_suote);
  tmpc[0] = '\0';

  asetelma();
  /*luetaan mahdollinen aloituskomentotiedosto*/
  tmpc[0] = '\0';
  char* apuc=tmpc; FILE* f; char c;
  if( aloituskomentotiedosto && (f=fopen(aloituskomentotiedosto, "r")) ) {
    while( (c = fgetc(f)) != EOF )
      *apuc++ = c;
    fclose(f);
  }
  /*luetaan komennot komentoriviltä*/
  for(int i=1; i<argc; i++) {
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
