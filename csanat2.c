#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "lista.h"
#include "näkymä.h"
#include "toiminta.h"
#include "modkeys.h"
#include "csanat2.h"

#define TAULPIT(a) ( sizeof(a) / sizeof(*(a)) )
#define LAITA(laitto) ( laitot |= (1<<laitto##_enum) )

typedef struct {
  char* sana;
  char* kaan;
  int id;
  int tiedostonro;
  lista* hetket; // (lista(uint32), jolla on kierroksilta)(hetki | osattu?<<31)
} snsto_1;

#if 0
typedef struct {
  char* kysym;
  char* syote;
} kysynnat_1;
#endif

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

lista snsto;
lista historia[3];
lista tiedostot;
SDL_Event tapaht;
unsigned modkey, laitot;
int kohdistin, syoteviesti;
char globchar[maxpit_syote];
char syotetxt[maxpit_syote];
char kysymtxt[maxpit_syote];

void aja();
void lopeta(Arg turha);
void jatka_syotetta(Arg char_p);
void napp_alas(Arg turha);
void napp_ylos(Arg turha);
void pyyhi_syotetta(Arg i_maara);

int utf8_siirto_eteen( const char* restrict str );
int utf8_siirto_taakse( const char* restrict str, int rmax );
void liita_teksti( char* s, char* liitos );

Sidonta sid_tapaht[] = {
  { SDL_QUIT,       0, lopeta,         {0}                    },
  { SDL_TEXTINPUT,  0, jatka_syotetta, {.v=&tapaht.text.text} },
  { SDL_KEYDOWN,    0, napp_alas,      {0}                    },
  { SDL_KEYUP,      0, napp_ylos,      {0}                    },
};

Sidonta sid_napp_alas[] = {
  { SDLK_BACKSPACE, 0, pyyhi_syotetta, {.i=-1} },
  { SDLK_DELETE,    0, pyyhi_syotetta, {.i=1}  },
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
  tuhoa_nakyma();
  tuhoa_tama_lista2(&tiedostot);
  tuhoa_tama_lista(&snsto);
  listastolla(tuhoa_lista2,historia,2);
  tuhoa_tama_lista(historia+2);
  exit(EXIT_SUCCESS);
}

void pyyhi_syotetta(Arg maara) {
  int pit = strlen(syotetxt);
  if(maara.i>0) {
    kohdistin -= utf8_siirto_eteen( syotetxt+pit-kohdistin );
    maara.i = -maara.i;
  }
  char* p2 = syotetxt+pit-kohdistin-1;
  char* p1 = p2 - utf8_siirto_taakse( syotetxt+pit-kohdistin, pit-kohdistin );
  while(( *++p1 = *++p2 ));
  LAITA(syote);
}

void jatka_syotetta(Arg arg_char_p) {
  char* mjon = arg_char_p.v;
  if(syoteviesti) { //syötteen paikalla voi olla viesti
    syotetxt[0] = '\0';
    syoteviesti = 0;
  }
  if(!kohdistin)
    strcat(syotetxt,mjon);
  else
    liita_teksti( syotetxt+strlen(syotetxt)-kohdistin, mjon );
  LAITA(syote);
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
  alusta_nakyma();
  alusta_tama_lista(&snsto,11,snsto_1);
  alusta_tama_lista(historia+0,11,char*);
  alusta_tama_lista(historia+1,11,char*);
  alusta_tama_lista(historia+2,11,uaika_t);
  alusta_tama_lista(&tiedostot, 1, char**);

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
  for(int i=0; i<laitot_enum_pituus; i++)
    laitot |= 1<<i;
  aja();

  puts("Varoitus: aja-funktio palasi, vaikka ei olisi pitänyt.");
  return 2;
}
