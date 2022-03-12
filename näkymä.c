#include "näkymä.h"
#include "csanat2.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int fkoko;
  const SDL_Color* etuvari;
  const SDL_Color* takavari;
  TTF_Font* font;
  SDL_Rect alue;
} nakyolio;

typedef struct {
  void* teksti;
  nakyolio* olio;
} piirtoarg;

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

void laita_teksti(piirtoarg arg) {
  if( !arg.teksti || !(strcmp(arg.teksti,"")) ) {
    arg.olio->alue.w = 0;
    arg.olio->alue.h = TTF_FontLineSkip(arg.olio->font);
    return;
  }
  SDL_Surface *pinta =  TTF_RenderUTF8_Shaded( arg.olio->font, arg.teksti, *arg.olio->etuvari, *arg.olio->takavari );
  if(!pinta) {
    fprintf(stderr, "Virhe TTF_RenderUTF8_Shaded: %s\n", TTF_GetError());
    return;
  }
  SDL_Texture *ttuuri =  SDL_CreateTextureFromSurface(rend, pinta);
  if(!ttuuri)
    fprintf(stderr, "Virhe SDL_CreateTextureFromSurface: %s\n", SDL_GetError());

  arg.olio->alue.w = arg.olio->alue.x+pinta->w > ikk_w ? ikk_w - arg.olio->alue.x : pinta->w;
  if( arg.olio->alue.y+pinta->h > ikk_h )
    goto LOPPU;
  arg.olio->alue.h = pinta->h;

  int w_minus = pinta->w - arg.olio->alue.w;
  int h_minus = pinta->h - arg.olio->alue.h;
  SDL_Rect kopioosa = { w_minus, h_minus, arg.olio->alue.w, arg.olio->alue.h };
  SDL_RenderCopy( rend, ttuuri, &kopioosa, &arg.olio->alue );
 LOPPU:
  SDL_FreeSurface(pinta);
  SDL_DestroyTexture(ttuuri);
}

void laita_listan_jasen(nakyolio* ol, char* teksti) {
  static int w, h;
  if(!teksti) {
    ol->alue.w = w;
    ol->alue.h = h;
    ol->alue.y -= h;
    w=0; h=0;
    return;
  }
  laita_teksti((piirtoarg){teksti,ol});
  if( ol->alue.w > w )
    w = ol->alue.w;
  ol->alue.y += ol->alue.h; //vaiko TTF_FontLineskip
  h += ol->alue.h;
}

void laita_historia(piirtoarg turha) {
  int i;
  int mahtuu = (ikk_h-histrol.alue.y) / TTF_FontLineSkip(histrol.font);
  int pienin = (mahtuu < historia[0]->pit) * (historia[0]->pit - mahtuu);
  for(i=historia[0]->pit-1; i>=pienin; i--) {
    histrol.takavari = &vo_taustavari[ *LISTALLA(historia[2],uaika_t*,i)>>sizeof(uaika_t)-1 ];
    laita_listan_jasen( &histrol, *LISTALLA(historia[0],char**,i) );
  }
  laita_listan_jasen(&histrol,NULL);
  SDL_Rect alue = histrol.alue;
  histrol.alue.x += histrol.alue.w;
  histrol.takavari = &taustavari;
  for(int j=historia[0]->pit-1; j>i; j--)
    laita_listan_jasen( &histrol, *LISTALLA(historia[1],char**,j) );
  laita_listan_jasen( &histrol, NULL );
  histrol.alue.x = alue.x;
  histrol.alue.w += alue.w;
  histrol.alue.h = alue.h > histrol.alue.h ? alue.h : histrol.alue.h;
}

void laita_lista(piirtoarg arg) {
  int pit = ((lista*)arg.teksti)->pit;
  int mahtuu = (ikk_w-arg.olio->alue.y) / TTF_FontLineSkip(arg.olio->font);
  for(int i=0; i<mahtuu; i++)
    laita_listan_jasen( arg.olio, *LISTALLA( ((lista*)arg.teksti), char**, i ) );
  laita_listan_jasen( arg.olio, NULL );
}

void (*piirtofunkt[])(piirtoarg) = {
  laita_teksti,
  laita_teksti,
  laita_historia,
  laita_lista,
  NULL,
};

piirtoarg piirtoargs[] = { { kysymtxt,  &kysymol },
			   { syotetxt,  &syoteol },
			   { NULL,      NULL,    },
			   { &tietolis, &tietool },
};

void paivita_kuva(unsigned laitot) {
  if(!laitot) {
    SDL_RenderCopy(rend,pohja,NULL,NULL);
    SDL_RenderPresent(rend);
    return;
  }
  for(int i=0; i<laitot_enum_pituus; i++)
    if( !(laitot>>i & 1) )
      piirtofunkt[i]( piirtoargs[i] );
}
