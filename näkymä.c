#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include "csanat2.h"

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

#define ASETA_VARI(c) SDL_SetRenderDrawColor( rend, (c).r, (c).g, (c).b, (c).a )
#define ASETA_ASIAN_VARI(olio,ind) (olio ## ol).etuvari = varit+ind;

static SDL_Window* ikk;
static SDL_Renderer* rend;
static SDL_Texture* pohja;

void paivita_sijainnit();
void paivita_ikkunan_koko();
void alusta_nakyma();
void tuhoa_nakyma();
void paivita_kuva(unsigned laitot);

#include "näkymä_conf1.c"

static void avaa_fontti(nakyolio* olio) {
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
  kohdistin_r = (SDL_Rect){.x=0, .y=syoteol.alue.y, .w=kohdistin_r.w, .h = TTF_FontLineSkip(syoteol.font)};
}

void paivita_ikkunan_koko() {
  SDL_GetWindowSize( ikk, &ikk_w, &ikk_h );
  SDL_DestroyTexture(pohja);
  pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikk_w, ikk_h );
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
  SDL_GetWindowSize( ikk, &ikk_w, &ikk_h );
  pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikk_w, ikk_h );

  avaa_fontti(&syoteol);
  avaa_fontti(&kysymol);
  avaa_fontti(&histrol);
  avaa_fontti(&tietool);

  paivita_sijainnit();
}

void tuhoa_nakyma() {
  TTF_CloseFont(syoteol.font);
  TTF_CloseFont(kysymol.font);
  TTF_CloseFont(histrol.font);
  TTF_CloseFont(tietool.font);
  SDL_DestroyTexture(pohja);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(ikk);
  TTF_Quit();
  SDL_Quit();
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
  int pienin = (mahtuu < historia[0].pit) * (historia[0].pit - mahtuu);
  for(i=historia[0].pit-1; i>=pienin; i--) {
    histrol.takavari = varit + ( *LISTALLA(historia+2,aika_t*,i) < 0 ? OIKEATAUSTV : VAARATAUSTV );
    laita_listan_jasen( &histrol, *LISTALLA(historia,char**,i) );
  }
  laita_listan_jasen(&histrol,NULL);
  SDL_Rect alue = histrol.alue;
  histrol.alue.x += histrol.alue.w;
  histrol.takavari = varit+TAUSTV;
  for(int j=historia[0].pit-1; j>i; j--)
    laita_listan_jasen( &histrol, *LISTALLA(historia+1,char**,j) );
  laita_listan_jasen( &histrol, NULL );
  histrol.alue.x = alue.x;
  histrol.alue.w += alue.w;
  histrol.alue.h = alue.h > histrol.alue.h ? alue.h : histrol.alue.h;
}

void laita_lista(piirtoarg arg) {
  int pit = ((lista*)arg.teksti)->pit;
  int mahtuu = (ikk_w-arg.olio->alue.y) / TTF_FontLineSkip(arg.olio->font);
  int loppu = mahtuu < pit ? mahtuu : pit;
  for(int i=0; i<loppu; i++)
    laita_listan_jasen( arg.olio, *LISTALLA( ((lista*)arg.teksti), char**, i ) );
  laita_listan_jasen( arg.olio, NULL );
}

void laita_kohdistin() {
  ASETA_VARI(varit[KOHDISTINV]);
  char* ptr = syotetxt+strlen(syotetxt)-kohdistin;
  char apu = *ptr;
  *ptr = '\0';
  TTF_SizeUTF8(syoteol.font, syotetxt, &kohdistin_r.x, NULL);
  *ptr = apu;
  SDL_RenderFillRect(rend, &kohdistin_r);
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
  if(laitot) {
    SDL_SetRenderTarget(rend,pohja);
    ASETA_VARI(varit[TAUSTV]);
    SDL_RenderClear(rend);
    for(int i=0; i<laitot_enum_pituus; i++)
      if( (laitot>>i & 1) )
	piirtofunkt[i]( piirtoargs[i] );
    laita_kohdistin();
    SDL_SetRenderTarget(rend,NULL);
  }
  SDL_RenderCopy(rend,pohja,NULL,NULL);
  SDL_RenderPresent(rend);
}
