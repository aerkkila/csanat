#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "csanat2.h"
#include "lista.h"
#include "toiminta.h"
#include "modkeys.h"

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

typedef struct {
  char* nimi;
  void (*funkt)(char*);
} Komento;

lista snsto;
lista historia[3];
lista tietolis;
lista tiedostot;
unsigned modkey, laitot;
int kohdistin, syoteviesti;
char globchar[maxpit_syote];
char syotetxt[maxpit_syote];
char kysymtxt[maxpit_syote];

void aja();
void lopeta(Arg turha);
void jatka_syotetta(Arg char_p);
void ikkunatapahtuma(Arg turha);
void napp_alas(Arg turha);
void napp_ylos(Arg turha);
void pyyhi_syotetta_eteen(Arg turha);
void pyyhi_syotetta_taakse(Arg turha);
void kohdistin_eteen(Arg maara);
void kohdistin_taakse(Arg maara);
void kasittele_syote(Arg syote);
void komento(Arg syote);

void lue_komento(char**);

int utf8_siirto_eteen( const char* restrict str );
int utf8_siirto_taakse( const char* restrict str, int rmax );
void liita_teksti( char* s, char* liitos );
aika_t* sana_historiaan();
void sana_oikein();
void sana_vaarin();

/*Kaikki johonkin tiettyyn grafiikka-alustaan liittyvä sisällytetään toisesta tiedostosta.
  Täällä taas käytetään vain alustasta (SDL, komentorivi, xlib, jne.) riippumattomia funktioita.*/
#include "näkymä.c"

SDL_Event tapaht;
/*Näissä tarkistus lopetetaan ensimmäiseen täsmäävään. -1 on mikä tahansa*/
Sidonta sid_tapaht[] = {
  { TAPAHT( KEYDOWN ),     -1,     napp_alas,       {0}                    },
  { TAPAHT( KEYUP ),       -1,     napp_ylos,       {0}                    },
  { TAPAHT( TEXTINPUT ),   0,      jatka_syotetta,  {.v=&tapaht.text.text} },
  { TAPAHT( TEXTINPUT ),   VAIHTO, jatka_syotetta,  {.v=&tapaht.text.text} },
  { TAPAHT( WINDOWEVENT ), -1,     ikkunatapahtuma, {0}                    },
  { TAPAHT( QUIT ),        -1,     lopeta,          {0}                    },
};

Sidonta sid_napp_alas[] = {
  { KEY( RETURN ),    0,   kasittele_syote,       {.v=syotetxt} },
  { KEY( KP_ENTER ),  0,   kasittele_syote,       {.v=syotetxt} },
  { KEY( BACKSPACE ), 0,   pyyhi_syotetta_taakse, {0}           },
  { KEY( DELETE ),    0,   pyyhi_syotetta_eteen,  {0}           },
  { KEY( g ),         ALT, kohdistin_taakse,      {.i=1}        },
  { KEY( o ),         ALT, kohdistin_eteen,       {.i=1}        },
};

#define KNTO(knto) .nimi = #knto, .funkt = knto ## _komento
Komento knnot[] = {
  { KNTO(lue) },
};

void aja() {
  SDL_StartTextInput();
  while(1) {
    while(SDL_PollEvent(&tapaht)) {
      for(int i=0; i<TAULPIT(sid_tapaht); i++)
	if( sid_tapaht[i].tyyppi == tapaht.type &&
	    ( sid_tapaht[i].mod == -1 || sid_tapaht[i].mod == modkey_tuplana(modkey) ) ) {
	  sid_tapaht[i].funktio(sid_tapaht[i].arg);
	  break;
	}
    }
    paivita_kuva(laitot);
    laitot = 0;
    SDL_Delay(uniaika);
  }
}

void ikkunatapahtuma(Arg turha) {
  if( tapaht.window.event == SDL_WINDOWEVENT_RESIZED )
    paivita_ikkunan_koko();
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
  listastolla(tuhoa_tama_lista2,historia,2);
  tuhoa_tama_lista(historia+2);
  tuhoa_tama_lista2(&tietolis);
  exit(EXIT_SUCCESS);
}

void pyyhi_syotetta_eteen(Arg turha) {
  int pit = strlen(syotetxt);
  char* p1 = syotetxt+pit-kohdistin;
  kohdistin -= utf8_siirto_eteen(p1);
  char* p2 = syotetxt+pit-kohdistin;
  while(( *p1++ = *p2++ ));
  LAITA(syote);
}

void pyyhi_syotetta_taakse(Arg turha) {
  int pit = strlen(syotetxt);
  char* p2 = syotetxt+pit-kohdistin;
  char* p1 = p2 - utf8_siirto_taakse( syotetxt+pit-kohdistin, pit-kohdistin );
  while(( *p1++ = *p2++ ));
  LAITA(syote);
}

void jatka_syotetta(Arg arg_char_p) {
  char* mjon = arg_char_p.v;
  if( strlen(syotetxt)+strlen(mjon) >= maxpit_syote )
    return;
  if(syoteviesti) { //syötteen paikalla voi olla viesti
    syotetxt[0] = '\0';
    syoteviesti = 0;
    ASETA_ASIAN_VARI(syote,ETUV);
  }
  if(!kohdistin)
    strcat(syotetxt,mjon);
  else
    liita_teksti( syotetxt+strlen(syotetxt)-kohdistin, mjon );
  LAITA(syote);
}

void kasittele_syote(Arg syotearg) {
  char* syote = syotearg.v-1;
  while(*++syote<=' ');
  if(*syote=='.')
    komento((Arg){.v=syote+1});
  else if( strcmp(kysymtxt,syotearg.v) )
    sana_vaarin();
  else
    sana_oikein();
}

void kohdistin_eteen(Arg maara) {
  for( int pit=strlen(syotetxt); maara.i--; kohdistin-=utf8_siirto_eteen(syotetxt+pit-kohdistin) );
  LAITA(syote);
}

void kohdistin_taakse(Arg maara) {
  for( int pit=strlen(syotetxt); maara.i--; kohdistin+=utf8_siirto_taakse( syotetxt+pit-kohdistin, pit-kohdistin ) );
  LAITA(syote);
}

void komento(Arg syotearg) {
  int sanoja = 0;
  char* restrict syote = syotearg.v;
  int luku;
  while(*syote) { //lasketaan sanat
    sanoja++;
    sscanf( syote, "%*s%n", &luku );
    syote += luku;
  }
  char* knnot[sanoja+1];
  syote = syotearg.v;
  for(int i=0; i<sanoja; i++) { //kopioidaan sanat
    sscanf( syote, "%*s%n", &luku );
    knnot[i] = malloc(luku+1);
    sscanf( syote, "%s", knnot[i] );
    syote += luku;
  }
  knnot[sanoja] = NULL;
  
  for( int i<0; i<TAULPIT(komennot); i++ ) {
    if( komennot[i].nimi == *knnot )
      komennot[i].funkt( syotearg.v, knnot );
  }

  for(int i=0; i<sanoja; i++)
    free(knnot[i]);

  *((char*)syotearg.v) = '\0';
  LAITA(syote);
}

int utf8_siirto_eteen( const char* restrict str ) {
  if(!*str)
    return 0;
  int r = 1;
  while( (*++str&0300) == 0200 )
    r++;
  return r;
}

int utf8_siirto_taakse( const char* restrict str, int rmax ) {
  int r = 0;
  if( r != rmax )
    while( r++ != rmax && (*--str&0300) == 0200);
  return r;
}

void liita_teksti( char* s, char* liitos ) {
  char loppu[strlen(s)+1];
  strcpy( loppu,  s );
  strcpy( s, liitos );
  strcat( s, loppu );
}

aika_t* sana_historiaan() {
  jatka_listaa( historia+2, 1 );
  aika_t *ptr = LISTALLA_LOPUSTA( historia+2, aika_t*, -1 );
  time((time_t*)ptr);
  listalle_kopioiden_mjon( historia+0, kysymtxt );
  listalle_kopioiden_mjon( historia+1, syotetxt );
  syoteviesti = 1;
  LAITA(historia);
  LAITA(syote);
  return ptr;
}

void sana_oikein() {
  aika_t* ptr = sana_historiaan();
  *ptr *= -1;
  ASETA_ASIAN_VARI(syote,OIKEAV);
}

void sana_vaarin() {
  sana_historiaan();
  ASETA_ASIAN_VARI(syote,VAARAV);
}

int main(int argc, char** argv) {
  alusta_nakyma();
  alusta_tama_lista(&snsto,11,snsto_1);
  alusta_tama_lista(historia+0,11,char*);
  alusta_tama_lista(historia+1,11,char*);
  alusta_tama_lista(historia+2,11,aika_t);
  alusta_tama_lista(&tiedostot, 1, char*);
  alusta_tama_lista(&tietolis, 3, char*);

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

  puts("\033[1;33mVaroitus:\033[0m aja-funktio palasi, vaikka ei olisi pitänyt.");
  return 2;
}
