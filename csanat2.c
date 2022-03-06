#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "lista.h"
//#include "asetelma.h"
#include "modkeys.h"

#define TAULPIT(a) ( sizeof(a) / sizeof(*(a)) )
#define LAITA(laitto) ( laitot |= (1<<laitto##_enum) )

enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  tiedot_enum,
  viesti_enum,
  laitot_enum_pituus,
};

typedef struct {
  char* sana;
  char* kaan;
  int id;
  int tiedostonro;
  lista* hetket; // (lista(uint32), jolla on kierroksilta)(hetki | osattu?<<31)
} snsto_1;

typedef struct {
  char* kysym;
  char* suote;
} kysynnat_1;

typedef union {
  int i;
  float f;
  void* v;
} Arg;

typedef struct {
  int tyyppi;
  unsigned mod;
  void (*funktio)(Arg);
  Arg arg;
} Sidonta;

SDL_Window *ikkuna;
SDL_Texture* tausta;
SDL_Renderer *rend;
lista* snsto;
lista* kysynnat;
lista* tiedostot;
SDL_Event tapaht;
unsigned modkey, laitot;
int kohdistin;
char* globchar;

void aja();
void lopeta(Arg turha);
void jatka_suotetta(Arg char_p);
void napp_alas(Arg turha);
void napp_ylos(Arg turha);
void pyyhi_suotetta(Arg i_maara);

int utf8_siirto_eteen( const char* restrict str );
int utf8_siirto_taakse( const char* restrict str, int rmax );
void liita_teksti( char* s, char* liitos );

Sidonta sid_tapaht[] = {
  { SDL_QUIT,       0, lopeta,         {0}                    },
  { SDL_TEXTINPUT,  0, jatka_suotetta, {.v=&tapaht.text.text} },
  { SDL_KEYDOWN,    0, napp_alas,      {0}                    },
  { SDL_KEYUP,      0, napp_ylos,      {0}                    },
};

Sidonta sid_napp_alas[] = {
  { SDLK_BACKSPACE, 0, pyyhi_suotetta, {.i=-1} },
  { SDLK_DELETE,    0, pyyhi_suotetta, {.i=1}  },
};

void aja() {
  SDL_StartTextInput();
  while(1) {
    while(SDL_PollEvent(&tapaht))
      for(int i=0; i<TAULPIT(sid_tapaht); i++)
	if( sid_tapaht[i].tyyppi == tapaht.type )
	  sid_tapaht[i].funktio(sid_tapaht[i].arg);
    paivita_kuva(laitot);
    SDL_Delay(uniaika);
  }
}

void napp_alas(Arg turha) {
  for(int i=0; i<TAULPIT(sid_napp_alas); i++)
    if( sid_napp_alas[i].tyyppi == tapaht.key.keysym.sym &&
	sid_napp_alas[i].mod    == modkey_tuplana(modkey) )
      sid_napp_alas[i].funktio(sid_napp_alas[i].arg);
  switch(tapaht.key.keysym.sym) {
#define _MODKEYS_SWITCH_KEYDOWN
#include "modkeys.h"
  }
}

void napp_ylos(Arg turha) {
  switch(tapaht.key.keysym.sym) {
#define _MODKEYS_SWITCH_KEYUP
#include "modkeys.h"
  }
}

void lopeta(Arg turha) {
  tiedostot = tuhoa_lista2(tiedostot);
  snsto     = tuhoa_lista(snsto);
  kysynnat  = tuhoa_lista(kysynnat);
  free(globchar);
  SDL_DestroyTexture(tausta);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(ikkuna);
  TTF_Quit();
  SDL_Quit();
  exit(EXIT_SUCCESS);
}

void pyyhi_suotetta(Arg maara) {
  int pit = strlen(suoteol.teksti);
  if(maara.i>0) {
    kohdistin -= utf8_siirto_eteen( suoteol.teksti+pit-kohdistin );
    maara.i = -maara.i;
  }
  char* p2 = suoteol.teksti+pit-kohdistin-1;
  char* p1 = p2 - utf8_siirto_taakse( suoteol.teksti+pit-kohdistin, pit-kohdistin );
  while(( *++p1 = *++p2 ));
  LAITA(suote);
}

void jatka_suotetta(Arg arg_char_p) {
  char* mjon = arg_char_p.v;
  if(suoteviesti) { //syötteen paikalla voi olla viesti
    suoteol.teksti[0] = '\0';
    suoteviesti = 0;
    suotekohdan_vari = suotevari;
  }
  if(!kohdistin)
    strcat(suoteol.teksti,mjon);
  else
    liita_teksti( suoteol.teksti+strlen(suoteol.teksti)-kohdistin, mjon );
  LAITA(suote);
}

int utf8_siirto_eteen( const char* restrict str ) {
  if(!*str)
    return 0;
  int r = 1;
  while( *++str&0300 == 0200 )
    r++;
  return r;
}

int utf8_siirto_taakse( const char* restrict str, int rmax ) {
  int r = 0;
  while( r != rmax && *--str&0300 == 0200 )
    r++;
  return r;
}

void liita_teksti( char* s, char* liitos ) {
  char loppu[strlen(s)+1];
  strcpy( loppu,  s );
  strcpy( s, liitos );
  strcat( s, loppu );
}

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
  globchar = malloc(maxpit_suote);

  //asetelma();
  SDL_GetWindowSize(ikkuna,&ikkuna_w,&ikkuna_h);
  asetelma_sijainnit(); //aseta_sijainnit();
  asetelma_kohdistin(); //saada_kohdistin();

  snsto     = alusta_lista(11,snsto_1   );
  kysynnat  = alusta_lista(11,kysynnat_1);
  tiedostot = alusta_lista(1, char**    );

  /*luetaan mahdollinen aloituskomentotiedosto*/
  globchar[0] = '\0';
  char* globchar2 = globchar; FILE* f; char c;
  if( aloituskomentotiedosto && (f=fopen(aloituskomentotiedosto, "r")) ) {
    while( (c = fgetc(f)) != EOF )
      *globchar2++ = c;
    fclose(f);
  }
  if(globchar2 != globchar) {
    if(*(globchar2-1) == '\n')
      *(globchar2-1) = '\0';
    else
      *globchar2 = '\0';
  }
  /*luetaan komennot komentoriviltä*/
  for(int i=1; i<argc; i++) {
    globchar2 = globchar + strlen(globchar);
    sprintf(globchar2, " %s", argv[i]);
  }
  aseta_vari(taustavari);
  SDL_RenderClear(rend);
  SDL_RenderPresent(rend);

  aja();

  puts("Varoitus: aja-funktio palasi, vaikka ei olisi pitänyt.");
  return 2;
}
