#include "näkymä.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int fkoko;
  const SDL_Color* etuvari;
  const SDL_Color* takavari;
  TTF_Font* font;
  SDL_Rect alue;
} nakyolio;

SDL_Window* ikk;
SDL_Renderer* rend;
SDL_Texture* pohja;
void avaa_fontti(nakyolio*);

int ikk_x=300, ikk_y=0, ikk_w=600, ikk_h=400;
const SDL_Color etuvari         = {255,255,255,255};
const SDL_Color taustavari      = {0,  0,  0,  0  };
const SDL_Color vo_vari[]       = { {20, 255,20, 255},
				    {255,20, 20, 255}, };
const SDL_Color vo_taustavari[] = { {20, 255,60, 255},
				    {255,80, 50, 255}, };
const SDL_Color kohdistinvari   = {255,255,255,255};
nakyolio syoteol = { .fkoko=60, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio kysymol = { .fkoko=60, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio histrol = { .fkoko=20, .etuvari=&etuvari, .takavari=&taustavari };
nakyolio tietool = { .fkoko=17, .etuvari=&etuvari, .takavari=&taustavari };
static const char* fonttied = "MAKE_LIITÄ_MONOFONTTI";
static const char* ohjelman_nimi = "csanat";

void avaa_fontti(nakyolio* olio) {
  olio->font = TTF_OpenFont( fonttied, olio->fkoko );
  if(!olio->font) {
    fprintf( stderr, "Ei avattu fonttia \"%s\"\n%s\n", fonttied, TTF_GetError() );
    exit(EXIT_FAILURE);
  }
}

void paivita_sijainnit() {
  syoteol.alue.y = TTF_FontLineSkip(kysymol.font);
  histrol.alue.y = syoteol.alue.y + TTF_FontLineSkip(syoteol.font);
  tietool.alue.y = histrol.alue.y;
}

#define FUNK(fun,virhefun,...)						\
  do {									\
  if(fun(__VA_ARGS__))							\
    {									\
      fprintf( stderr, "Virhe funktiossa " #fun ": %s\n", virhefun() ); \
      exit(EXIT_FAILURE);						\
    }									\
  } while(0)

void alusta_nakyma() {
  FUNK(SDL_Init,SDL_GetError,SDL_INIT_VIDEO);
  FUNK(TTF_Init,TTF_GetError);
  ikk = SDL_CreateWindow( ohjelman_nimi, ikk_x, ikk_y, ikk_w, ikk_h, SDL_WINDOW_RESIZABLE );
  rend = SDL_CreateRenderer( ikk, -1, SDL_RENDERER_TARGETTEXTURE );
  pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikk_w, ikk_h );

  avaa_fontti(&syoteol);
  avaa_fontti(&kysymol);
  avaa_fontti(&histrol);
  avaa_fontti(&tietool);

  SDL_GetWindowSize( ikk, &ikk_w, &ikk_h );
  paivita_sijainnit();
}

int laita_teksti(nakyolio* ol, char* teksti) {
  if( !teksti || !(strcmp(teksti,"")) ) {
    ol->alue.w = 0;
    ol->alue.h = TTF_FontLineSkip(ol->font);
    return 2;
  }
  SDL_Surface *pinta =  TTF_RenderUTF8_Shaded( ol->font, teksti, *ol->etuvari, *ol->takavari );
  if(!pinta) {
    fprintf(stderr, "Virhe TTF_RenderUTF8_Shaded: %s\n", TTF_GetError());
    return 3;
  }
  SDL_Texture *ttuuri =  SDL_CreateTextureFromSurface(rend, pinta);
  if(!ttuuri)
    fprintf(stderr, "Virhe SDL_CreateTextureFromSurface: %s\n", SDL_GetError());

  ol->alue.w = ol->alue.x+pinta->w > ikk_w ? ikk_w - ol->alue.x : pinta->w;
  int taynna = ol->alue.y+pinta->h > ikk_h;
  if(taynna)
    goto LOPPU;
  ol->alue.h = pinta->h;

  int w_minus = pinta->w - ol->alue.w;
  int h_minus = pinta->h - ol->alue.h;
  SDL_Rect kopioosa = { w_minus, h_minus, ol->alue.w, ol->alue.h };
  SDL_RenderCopy( rend, ttuuri, &kopioosa, &ol->alue );
 LOPPU:
  SDL_FreeSurface(pinta);
  SDL_DestroyTexture(ttuuri);
  return taynna;
}

int laita_listan_jasen(nakyolio* ol, char* teksti) {
  static int w, h;
  if(!teksti) {
    ol->alue.w = w;
    ol->alue.h = h;
    ol->alue.y -= h;
    w=0; h=0;
    return -1;
  }
  if( laita_teksti(ol, teksti) )
    return 1;
  if( ol->alue.w > w )
    w = ol->alue.w;
  ol->alue.y += ol->alue.h; //vaiko TTF_FontLineskip
  h += ol->alue.h;
  return 0;
}

void laita_historia() {
  int i;
  for(i=historia[0].pit-1; i>=0;) {
    histrol.takavari = vo_taustavari[ *LISTALLA(historia[2],uaika_t,i)>>sizeof(uaika_t)-1 ];
    if(laita_listan_jasen( histrol, *LISTALLA(historia[0],char**,i--) ))
      break;
  }
  laita_listan_jasen(histrol,NULL);
  SDL_Rect alue = histrol.alue;
  histrol.x += histrol.w;
  histrol.takavari = taustavari;
  for(int j=historia[0].pit-1; j>i; j--)
    laita_listan_jasen( histrol, *LISTALLA(historia[1],char**,j) );
  laita_listan_jasen( histrol, NULL );
  histrol.x = alue.x;
  histrol.w += alue.w;
  histrol.h = alue.h > histrol.alue.h ? alue.h : histrol.alue.h;
}

void (*piirtofunkt)(void**)[] = {
  laita_teksti,
  laita_teksti,
  laita_historia,
  laita_lista,
};

nakyolio oliot[] = { &kysymol, &syoteol, NULL, &tietool };

void paivita_kuva(unsigned laitot) {
  if(!laitot) {
    SDL_RenderCopy(rend,pohja,NULL,NULL);
    SDL_RenderPresent(rend);
    return;
  }
  for(int i=0; i<laitot_enum_pituus; i++)
    if( !(laitot>>i & 1) )
      piirtofunkt[i](oliot+i*2);
}
