#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include "csanat.h"

typedef struct {
  int fkoko;
  const SDL_Color* etuvari;
  const SDL_Color* takavari;
  TTF_Font* font;
  SDL_Rect alue;
  SDL_Texture* pohja;
} nakyolio;

typedef struct {
  void* teksti;
  nakyolio* olio;
} piirtoarg;

#define ASETA_VARI(c) SDL_SetRenderDrawColor( rend, (c).r, (c).g, (c).b, (c).a )
#define ASETA_ASIAN_VARI(olio,kumpi,ind) (olio ## ol).kumpi ##vari = varit+ind;
#define ASETA_ASIAN_VARIT(olio,ind1,ind2) do {	\
    (olio ## ol).etuvari = varit+ind1;		\
    (olio ## ol).takavari = varit+ind2;		\
  } while(0)
#define TAPAHT(a) SDL_ ## a
#define KEY(a) SDLK_ ## a

static SDL_Window* ikk;
static SDL_Renderer* rend;
static SDL_Texture* pohja;
static int uusi_pohjat;

static void laita_teksti(piirtoarg);
static void laita_historia(piirtoarg);
static void laita_lista(piirtoarg);
static void laita_listan_jasen(nakyolio*, char*, int);
static void paivita_sijainnit();
static void paivita_ikkunan_koko();
static void valmista_nakyolion_tekstuuri(nakyolio*);
void alusta_nakyma();
void tuhoa_nakyma();
void paivita_kuva(unsigned laitot);

#include "näkymä_conf1.c"

void (*piirtofunkt[])(piirtoarg) = {
  laita_teksti,
  laita_teksti,
  laita_historia,
  laita_lista,
  NULL,
};

piirtoarg piirtoargs[] = { { kysymtxt,  &kysymol },
			   { syotetxt,  &syoteol },
			   { NULL,      &histrol },
			   { &tietolis, &tietool },
};

static void avaa_fontti(nakyolio* olio) {
  olio->font = TTF_OpenFont( fonttied, olio->fkoko );
  if(!olio->font) {
    fprintf( stderr, "Ei avattu fonttia \"%s\"\n%s\n", fonttied, TTF_GetError() );
    exit(EXIT_FAILURE);
  }
}

static void paivita_sijainnit() {
  syoteol.alue.y = TTF_FontLineSkip(kysymol.font);
  histrol.alue.y = syoteol.alue.y + TTF_FontLineSkip(syoteol.font);
  tietool.alue.y = histrol.alue.y;
  kohdistin_r = (SDL_Rect){.x=0, .y=syoteol.alue.y, .w=kohdistin_r.w, .h = TTF_FontLineSkip(syoteol.font)};
}

static void paivita_ikkunan_koko() {
  SDL_GetWindowSize( ikk, &ikk_w, &ikk_h );
  SDL_DestroyTexture(pohja);
  pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ikk_w, ikk_h );
  uusi_pohjat = 1;
}

#define FUNK(fun,virhefun,...)						\
  do {									\
  if(fun(__VA_ARGS__))							\
    {									\
      fprintf( stderr, "Virhe funktiossa " #fun ": %s\n", virhefun() ); \
      exit(EXIT_FAILURE);						\
    }									\
  } while(0)

static void laita_teksti(piirtoarg arg) {
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
  SDL_Rect kopioosa = { w_minus, 0, arg.olio->alue.w, arg.olio->alue.h };
  SDL_RenderCopy( rend, ttuuri, &kopioosa, &arg.olio->alue );
 LOPPU:
  SDL_FreeSurface(pinta);
  SDL_DestroyTexture(ttuuri);
}

static void laita_listan_jasen(nakyolio* ol, char* teksti, int jatka) {
  static int w, h;
  if(!jatka) {
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

static void laita_historia(piirtoarg turha) {
  int i, apu;
  int mahtuu = (ikk_h-histrol.alue.y) / TTF_FontLineSkip(histrol.font);
  int pienin = (mahtuu < historia[0].pit) * (historia[0].pit - mahtuu);

  /*vasen lista*/
  for(i=historia[0].pit-1; i>=pienin; i--) {
    if ( *LISTALLA(historia+2,aika_t*,i) & (aika_t)1<<(sizeof(aika_t)*8-1) )
      ASETA_ASIAN_VARIT(histr,O_HIST1,O_HIST2);
    else
      ASETA_ASIAN_VARIT(histr,V_HIST1,V_HIST2);
    laita_listan_jasen( &histrol, *LISTALLA(historia,char**,i), 1);
  }
  laita_listan_jasen(&histrol,NULL,0);
  
  SDL_Rect alue = histrol.alue;
  TTF_SizeUTF8(histrol.font,"  ",&apu,NULL);
  histrol.alue.x += histrol.alue.w + 2*apu;
  histrol.takavari = varit+TAUSTV;
  
  /*oikea lista*/
  ASETA_ASIAN_VARIT(histr,ETUV,TAUSTV);
  for(int j=historia[0].pit-1; j>i; j--)
    laita_listan_jasen( &histrol, *LISTALLA(historia+1,char**,j), 1 );
  laita_listan_jasen( &histrol, NULL, 0 );
  
  histrol.alue.w += histrol.alue.x-alue.x;
  histrol.alue.x = alue.x;
  histrol.alue.h = alue.h > histrol.alue.h ? alue.h : histrol.alue.h;
}

static void laita_lista(piirtoarg arg) {
  int pit = ((lista*)arg.teksti)->pit;
  int mahtuu = (ikk_w-arg.olio->alue.y) / TTF_FontLineSkip(arg.olio->font);
  int loppu = mahtuu < pit ? mahtuu : pit;
  for(int i=0; i<loppu; i++)
    laita_listan_jasen( arg.olio, *LISTALLA( ((lista*)arg.teksti), char**, i ), 1 );
  laita_listan_jasen( arg.olio, NULL, 0 );
  /*kopiointi varsinaiseen tekstuuriin*/
  SDL_SetRenderTarget( rend, pohja );
  SDL_RenderCopy( rend, arg.olio->pohja, &arg.olio->alue, &arg.olio->alue );
}

static void valmista_nakyolion_tekstuuri(nakyolio* olio) {
  if(olio->pohja) {
    if(uusi_pohjat) {
      SDL_DestroyTexture(olio->pohja);
      olio->pohja = NULL;
      goto EI_POHJAA;
    }
    SDL_SetRenderTarget( rend, olio->pohja );
    ASETA_VARI(varit[TAUSTV]);
    SDL_RenderClear(rend);
    return;
  }
 EI_POHJAA:
  olio->pohja = SDL_CreateTexture( rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
				   ikk_w, ikk_h );
  SDL_SetRenderTarget( rend, olio->pohja );
}

static void laita_kohdistin() {
  ASETA_VARI(varit[KOHDISTINV]);
  char* ptr = syotetxt+strlen(syotetxt)-kohdistin;
  char apu = *ptr;
  *ptr = '\0';
  TTF_SizeUTF8(syoteol.font, syotetxt, &kohdistin_r.x, NULL);
  *ptr = apu;
  SDL_RenderFillRect(rend, &kohdistin_r);
}

void alusta_nakyma() {
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
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
  int pit = TAULPIT(piirtoargs);
  for(int i=0; i<pit; i++)
    if(piirtoargs[i].olio->pohja) {
      SDL_DestroyTexture(piirtoargs[i].olio->pohja);
      piirtoargs[i].olio->pohja = NULL;
    }
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

void paivita_kuva(unsigned laitot) {
  if(laitot | uusi_pohjat) {
    SDL_SetRenderTarget(rend,pohja);
    ASETA_VARI(varit[TAUSTV]);
    SDL_RenderClear(rend);
    for(int i=0; i<laitot_enum_pituus; i++) {
      if( (laitot & 1<<i) | uusi_pohjat ) {
	if(i==tieto_enum) {
	  nakyolio* ol = piirtoargs[historia_enum].olio;
	  int apu;
	  TTF_SizeUTF8(histrol.font,"  ",&apu,NULL);
	  piirtoargs[i].olio->alue.x = ol->alue.x + ol->alue.w + apu;
	}
	valmista_nakyolion_tekstuuri(piirtoargs[i].olio);
	piirtofunkt[i]( piirtoargs[i] );
	SDL_SetRenderTarget(rend,pohja);
      }
      SDL_RenderCopy( rend, piirtoargs[i].olio->pohja, &piirtoargs[i].olio->alue, &piirtoargs[i].olio->alue );
    }
    laita_kohdistin();
    SDL_SetRenderTarget(rend,NULL);
  }
  uusi_pohjat = 0;
  SDL_RenderCopy(rend,pohja,NULL,NULL);
  SDL_RenderPresent(rend);
}
