#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "csanat2.h"
#include "lista.h"
#include "toiminta.h"
#include "modkeys.h"

#define TAULPIT(a) ( sizeof(a) / sizeof(*(a)) )
#define LAITA(laitto) ( laitot |= (1<<laitto##_enum) )

/* Tässä hetket viittaa lukuihin kuin historia+2, jotka luodaan historia+2-listaan.
   Listalla on (aika_t)aika, jossa merkitsevimmät bitit ovat (osattiin,kumpi_kysyttiin)*/
typedef struct {
  char* sana[2];
  //int id;
  //int tiedostonro;
  lista hetket;
} snsto_1;

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
int *kysymjarj, kysymjarjpit;
char globchar[maxpit_syote];
char syotetxt[maxpit_syote];
char kysymtxt[maxpit_syote];
unsigned modkey, laitot;
int kohdistin, syoteviesti, kumpi_kysym, kysymind;

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
void shellkomento(Arg syote);

/*Nämä funktiot lisätään knnot-taulukkoon, KNTO-makrolla.*/
void komento_lue(char*);

void lue_sanastoksi(char* tnimi);
char* lue_tiedosto_merkkijonoksi(char* tnimi);
void luo_uusi_jarjestys(int** jarj, int* jarjpit);
int laske_osaamiset(lista* hetket);
int utf8_siirto_eteen( const char* restrict str );
int utf8_siirto_taakse( const char* restrict str, int rmax );
void liita_teksti( char* s, char* liitos );
void knto_historiaan(char* knto);
void sana_historiaan(int oikeinko);
void viestiksi(char*);
void laita_kysymys(int ind);
void laita_tiedot();

/*Kaikki johonkin tiettyyn grafiikka-alustaan liittyvä sisällytetään toisesta tiedostosta.
  Täällä taas käytetään vain alustasta (SDL, komentorivi, xlib, jne.) riippumattomia funktioita.
  Käytännössä ei taida oikein toteutua, mutta tuo on pyrkimys.*/
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

#define KNTO(knto) .nimi = #knto, .funkt = komento_ ## knto
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
  for(int i=0; i<snsto.pit; i++)
    tuhoa_tama_lista(&LISTALLA(&snsto,snsto_1*,i)->hetket);
  tuhoa_tama_lista(&snsto);
  listastolla(tuhoa_tama_lista2,historia,2);
  tuhoa_tama_lista(historia+2);
  tuhoa_tama_lista2(&tietolis);
  free(kysymjarj);
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
  if(*syote == komentomerkki)
    komento((Arg){.v=syote+1});
  else if(*syote == shellkomentomerkki)
    shellkomento((Arg){.v=syote+1});
  else if(snsto.pit) {
    sana_historiaan( !strcmp(kysymtxt,--syote) );
    laita_kysymys(++kysymind);
    return;
  }
  knto_historiaan(syote);
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
  int pit;
  sscanf(syotearg.v, "%*s%n", &pit);
  char knto[pit+1];
  memcpy(knto, syotearg.v, pit);
  knto[pit] = 0;
  pit = TAULPIT(knnot);
  for( int i=0; i<pit; i++ )
    if( !strcmp(knnot[i].nimi,knto) )
      knnot[i].funkt(syotearg.v);
  *((char*)syotearg.v) = '\0';
  LAITA(syote);
}

void shellkomento(Arg syotearg) {
  char* syote = syotearg.v;
  tuhoa_tama_lista2(&tietolis);
  alusta_tama_lista(&tietolis,8,char**);
  FILE *f = popen(syote,"r");
  if(!f) {
    perror("popen");
    viestiksi("Ei avattu prosessia \n");
  } else {
    char sana[tieto_nchar];
    int i=0;
    while(!feof(f)) {
      sana[i] = fgetc(f);
      if( sana[i] != '\n' ) {
	if( ++i < tieto_nchar )
	  continue;
	fseek(f,-1,SEEK_CUR);
      }
      sana[i] = '\0';
      listalle_kopioiden_mjon(&tietolis,sana);
      i=0;
    }
    pclose(f);
  }
  *(char*)syotearg.v = '\0';
  LAITA(tieto);
}

void komento_lue(char* syote) {
  int luku;
  sscanf(syote, "%*s%n", &luku);
  syote += luku;
  char sana[512];
  while( sscanf(syote, "%511s%n", sana, &luku) == 1 ) {
    lue_sanastoksi(sana);
    syote += luku;
  }
  luo_uusi_jarjestys(&kysymjarj,&kysymjarjpit);
  kysymind = 0;
  laita_kysymys(kysymind);
}

void lue_sanastoksi(char* tnimi) {
  unsigned char* tied = (unsigned char*)lue_tiedosto_merkkijonoksi(tnimi);
  if(!tied)
    return;
  int i=-1;
  /*Tiedosto on nyt luettu yhdeksi merkkijonoksi.
    Tässä ei enää kopioida mitään,
    vaan laitetaan viitteet kyseisen merkkijonon sananalkukohtiin
    ja muutetaan sanojen erotinmerkit nollatavuiksi.*/
  int joko0tai1 = 0;
  while(1) {
    while(tied[++i] && tied[i] <= ' ');
    if(!tied[i])
      return;
    /*sanan alku löytyi ja laitetaan listalle*/
    snsto_1* jasen = jatka_listaa(&snsto,!joko0tai1); // if(!joko0tai1) jatka_listaa(lista,1)
    jasen->sana[joko0tai1] = (char*)tied+i;
    alusta_tama_lista(&jasen->hetket,4,aika_t**);
    joko0tai1 = (joko0tai1+1) % 2;
    while(tied[++i] >= ' ');
    if(!tied[i])
      return;
    tied[i] = '\0';
  }
}

char* lue_tiedosto_merkkijonoksi(char* tnimi) {
  int fd;
  struct stat filestat;
  char* tied = NULL;
  if( (fd = open(tnimi,O_RDONLY))      < 0 ||
      fstat(fd,&filestat)              < 0 ||
      (tied = malloc(filestat.st_size+1)) == NULL ||
      read(fd,tied,filestat.st_size)   < 0 )
    perror("lue_tiedosto");
  else
    tied[filestat.st_size] = '\0';
  if( close(fd) < 0 )
    perror("lue_tiedosto, close");
  return tied;
}

void luo_uusi_jarjestys(int** jarj, int* jarjpit) {
  free(*jarj); *jarj = NULL;
  lista jarjlis;
  alusta_tama_lista(&jarjlis, 8, int);
  for(int i=0; i<snsto.pit; i++)
    if(laske_osaamiset(&LISTALLA(&snsto,snsto_1*,i)->hetket) < osaamisraja)
      *(int*)jatka_listaa(&jarjlis,1) = i;
  *jarj = jarjlis.taul; //listan kasamuistiosa sijoitetaan tähän, joten ei tarvitse vapauttaa
  *jarjpit = jarjlis.pit;
  //sekoittaminen puuttuu
}

int laske_osaamiset(lista* hetket) {
  aika_t** taul = hetket->taul;
  int osaamiset = 0;
  for(int i=0; i<hetket->pit; i++)
    if( *taul[i] & 1<<(sizeof(aika_t)-1) )
      osaamiset++;
  return osaamiset;
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

void knto_historiaan(char* knto) {
  *(char**)jatka_listaa(historia+0, 1) = NULL;
  listalle_kopioiden_mjon(historia+1, knto);
  time( (time_t*)jatka_listaa(historia+2, 1) );
  LAITA(historia);
}

void sana_historiaan(int oikeinko) {
  listalle_kopioiden_mjon( historia+0, kysymtxt );
  listalle_kopioiden_mjon( historia+1, syotetxt );
  /*historia+2 on lista hetkistä ja osaamisista*/
  aika_t* ptr = jatka_listaa( historia+2, 1 );
  time((time_t*)ptr);
  *ptr |= oikeinko<<(sizeof(aika_t)-1);
  *ptr |= kumpi_kysym<<(sizeof(aika_t)-2);
  /*snsto_1.hetket viittaa samaan kuin historia+2*/
  lista* snstohetk = &LISTALLA(&snsto,snsto_1*,kysymjarj[kysymind])->hetket;
  *(aika_t**)jatka_listaa(snstohetk,1) = ptr;
  /*näkymän asiat*/
  ASETA_ASIAN_VARI(syote,VAARAV+oikeinko);
  syoteviesti = 1;
  LAITA(historia);
  LAITA(syote);
}

void viestiksi(char* syote) {
  tuhoa_tama_lista2(&tietolis);
  alusta_tama_lista(&tietolis,4,char**);
  char sana[tieto_nchar+1];
  int j=0, i=0;
  while(1) { //tätä pitää tarkastella vielä
    sana[j] = syote[i];
    if(++j == tieto_nchar || sana[j] == '\n') {
      sana[j] = '\0';
      j = 0;
      listalle_kopioiden_mjon(&tietolis,sana);
    }
    if(syote[++i])
      continue;
    break;
  }
  LAITA(tieto);
}

void laita_kysymys(int ind) {
  if( ind == kysymjarjpit ) {
    *kysymtxt = '\0';
    return;
  } else if( ind > kysymjarjpit ) {
    ind = 0;
    luo_uusi_jarjestys(&kysymjarj,&kysymjarjpit);
  }
  char* kopioitava = LISTALLA(&snsto,snsto_1*,kysymjarj[ind])->sana[kumpi_kysym];
  int i;
  for(i=0; (i<maxpit_syote-1) & kopioitava[i]; i++)
    kysymtxt[i] = kopioitava[i];
  kysymtxt[i] = '\0';
  LAITA(kysymys);
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
  while(SDL_PollEvent(&tapaht)); //tyhjennetään
  paivita_ikkunan_koko();
  aja();

  puts("\033[1;33mVaroitus:\033[0m aja-funktio palasi, vaikka ei olisi pitänyt.");
  return 2;
}
