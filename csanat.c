#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "csanat.h"
#include "lista.h"
#include "toiminta.h"
#include "modkeys.h"

#define TAULPIT(a) ( sizeof(a) / sizeof(*(a)) )
#define LAITA(laitto) ( laitot |= (1<<laitto##_enum) )

/* Tässä hetket viittaa lukuihin, jotka luodaan historia+2-listaan.
   Listalla on (aika_t)aika, jossa merkitsevimmät bitit ovat (osattiin,kumpi_kysyttiin)
   id:t ovat negatiivisia sanoille, joita ei ole tallennettu io_tiedot-tiedoston menetelmillä.*/
typedef struct {
  char* sana[2];
  int id;
  char* tiedosto;
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

lista snsto, historia[3], tietolis, tiedostot;
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
void sigtrap(Arg turha);
void kasittele_syote(Arg syote);
void osatuksi(Arg suht_sij);
void komento(Arg syote);
void shellkomento(Arg syote);

/*Nämä funktiot lisätään knnot-taulukkoon, KNTO-makrolla.*/
void komento_lue(char*);
void komento_tulosta_historia(char*);

void lue_sanastoksi(char* tnimi);
char* lue_tiedosto_merkkijonoksi(char* tnimi);
void sekoita(int* lista, int pit);
void luo_uusi_jarjestys(int** jarj, int* jarjpit);
int laske_osaamiset(lista* hetket);
int utf8_siirto_eteen( const char* restrict str );
int utf8_siirto_taakse( const char* restrict str, int rmax );
void liita_teksti( char* s, char* liitos );
void knto_historiaan(char* knto);
void kasittele_yrite(int oikeinko);
void viestiksi(char*);
void kasittele_kysymys();
void tee_tiedot();

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
  { KEY( RETURN ),    ALT, osatuksi,              {.i=-1}       },
  { KEY( KP_ENTER ),  ALT, osatuksi,              {.i=-1}       },
  { KEY( BACKSPACE ), 0,   pyyhi_syotetta_taakse, {0}           },
  { KEY( DELETE ),    0,   pyyhi_syotetta_eteen,  {0}           },
  { KEY( g ),         ALT, kohdistin_taakse,      {.i=1}        },
  { KEY( o ),         ALT, kohdistin_eteen,       {.i=1}        },
  { KEY( PAUSE ),     ALT, sigtrap,               {0}           },
};

#define KNTO(knto) .nimi = #knto, .funkt = komento_ ## knto
Komento knnot[] = {
  { KNTO(lue) },
  { KNTO(tulosta_historia) },
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
  for(int i=0; i<snsto.pit; i++)
    tuhoa_tama_lista(&LISTALLA(&snsto,snsto_1*,i)->hetket);
  tuhoa_tama_lista(&snsto);
  listastolla(tuhoa_tama_lista2,historia,2);
  tuhoa_tama_lista(historia+2);
  tuhoa_tama_lista2(&tietolis);
  tuhoa_tama_lista2(&tiedostot);
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
    ASETA_ASIAN_VARIT(syote,ETUV,TAUSTV);
  }
  if(!kohdistin)
    strcat(syotetxt,mjon);
  else
    liita_teksti( syotetxt+strlen(syotetxt)-kohdistin, mjon );
  LAITA(syote);
}

void kasittele_syote(Arg syotearg) {
  LAITA(syote);
  LAITA(tieto);
  LAITA(historia);
  char* syote = syotearg.v-1;
  kohdistin = 0;
  while(*++syote<=' ');
  char* seur = syote;
  while(*seur && *seur != '\n')
    seur++;
  if(*seur == '\n') {
    *seur = '\0';
    seur++;
  }
  int onkoseur = !!(*seur); //tallennetaan nyt, koska syötettä voidaan muokata oikeaksi vastaukseksi
  if(*syote == komentomerkki) {
    komento((Arg){.v=syote+1});
    knto_historiaan(syote);
    ((char*)syotearg.v)[0] = '\0';
  } else if(*syote == shellkomentomerkki) {
    shellkomento((Arg){.v=syote+1});
    knto_historiaan(syote);
    ((char*)syotearg.v)[0] = '\0';
  } else if(snsto.pit) {
    if(kysymind < kysymjarjpit)
      kasittele_yrite(!strcmp(LISTALLA(&snsto,snsto_1*,kysymjarj[kysymind])->sana[!kumpi_kysym], syote));
    kysymind++;
    kasittele_kysymys();
    tee_tiedot();
  }
  if(onkoseur)
    kasittele_syote((Arg){.v=seur});
}

void osatuksi(Arg suht_sij) {
  if(!kysymind || !kysymjarjpit)
    return;
  aika_t *ptr = LISTALLA_LOPUSTA(historia+2,aika_t*,suht_sij.i);
  *ptr |= (aika_t)1<<(sizeof(aika_t)*8-1);
  tee_tiedot();
  LAITA(historia);
}

#define _LISTALLE(i,muoto,...)						\
  {									\
    sprintf(apuc, muoto, __VA_ARGS__);					\
    *LISTALLA(&tietolis, char**, i) = strndup(apuc,tieto_nchar+1);	\
    if(strlen(apuc) > tieto_nchar)					\
      *LISTALLA(&tietolis, char**, i)[tieto_nchar] = '\0';		\
  }
void tee_tiedot() {
  char apuc[256];
  int osattuja = 0;
  for(int i=0; i<snsto.pit; i++)
    osattuja += laske_osaamiset(&LISTALLA(&snsto,snsto_1*,i)->hetket) >= osaamisraja;
  tuhoa_tama_lista2(&tietolis);
  jatka_listaa(&tietolis,3);
  _LISTALLE(0, "Sijainti %i/%i", kysymind, kysymjarjpit);
  _LISTALLE(1, "Osattuja %i/%i", osattuja, snsto.pit);
  _LISTALLE(2, "Tiedosto: \"%s\"", LISTALLA(&snsto, snsto_1*, kysymjarj[kysymind])->tiedosto);
  LAITA(tieto);
}
#undef _LISTALLE

void kohdistin_eteen(Arg maara) {
  for( int pit=strlen(syotetxt); maara.i--; kohdistin-=utf8_siirto_eteen(syotetxt+pit-kohdistin) );
  LAITA(syote);
}

void kohdistin_taakse(Arg maara) {
  for( int pit=strlen(syotetxt); maara.i--; kohdistin+=utf8_siirto_taakse( syotetxt+pit-kohdistin, pit-kohdistin ) );
  LAITA(syote);
}

void sigtrap(Arg turha) {
  asm("int $3");
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
  kasittele_kysymys(kysymind);
}

void komento_tulosta_historia(char* syote) {
  const char* a = "\033[94m";
  const char* b = "\033[95m";
  for(int i=0; i<historia[2].pit; i++)
    printf( "%s%s\t%s%s\n",
	    a, *LISTALLA(historia+0,char**,i),
	    b, *LISTALLA(historia+1,char**,i) );
  puts("\033[0m");
}

void lue_sanastoksi(char* tnimi) {
  static int id = -1; //tiedostosta luetut ovat negatiivisia
  unsigned char* tied = (unsigned char*)lue_tiedosto_merkkijonoksi(tnimi);
  if(!tied)
    return;
  int i=-1;
  char** tiednimi = jatka_listaa(&tiedostot, 1);
  *tiednimi = strdup(tnimi);
  /*Tiedosto on nyt luettu yhdeksi merkkijonoksi.
    Tässä ei enää kopioida mitään,
    vaan laitetaan viitteet kyseisen merkkijonon sanojen alkukohtiin
    ja muutetaan sanojen erotinmerkit nollatavuiksi.*/
  int joko0tai1 = 0;
  while(1) {
    while(tied[++i] && tied[i] <= ' ');
    if(!tied[i])
      return;
    /*sanan alku löytyi ja laitetaan listalle*/
    snsto_1* jasen = jatka_listaa(&snsto,!joko0tai1); // if(!joko0tai1) jatka_listaa(lista,1)
    if(!joko0tai1) {
      alusta_tama_lista(&jasen->hetket,4,aika_t**);
      jasen->id = id--;
      jasen->tiedosto = *tiednimi;
    }
    jasen->sana[joko0tai1] = (char*)tied+i;
    joko0tai1 = !joko0tai1;

    while(tied[++i] >= ' ');
    if(!tied[i])
      return;
    if(joko0tai1 && tied[i] != '\n')
      /*Tällöin kyse on lisämääritteestä. Tämä ohjelma ei vielä käsittele niitä.*/
      while(tied[++i] && tied[i] != '\n');
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

void sekoita(int* lis, int pit) {
  if(!pit)
    return;
  int *loppu = lis+pit;
  while(lis < loppu-1) {
    int sij = rand()%(loppu-lis);
    VAIHDA(lis[0], lis[sij], int);
    lis++;
  }
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
  sekoita(*jarj, *jarjpit);
}

int laske_osaamiset(lista* hetket) {
  aika_t** taul = hetket->taul;
  int osaamiset = 0;
  for(int i=0; i<hetket->pit; i++)
    if( *taul[i] & (aika_t)1<<(sizeof(aika_t)*8-1) )
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

void kasittele_yrite(int oikeinko) {
  listalle_kopioiden_mjon( historia+0, kysymtxt );
  listalle_kopioiden_mjon( historia+1, syotetxt );
  /*historia+2 on lista hetkistä ja osaamisista*/
  aika_t* ptr = jatka_listaa( historia+2, 1 );
  time((time_t*)ptr);
  *ptr |= (aika_t)oikeinko<<(sizeof(aika_t)*8-1);
  *ptr |= (aika_t)kumpi_kysym<<(sizeof(aika_t)*8-2);
  /*snsto_1.hetket viittaa samaan kuin historia+2*/
  lista* snstohetk = &LISTALLA(&snsto,snsto_1*,kysymjarj[kysymind])->hetket;
  *(aika_t**)jatka_listaa(snstohetk,1) = ptr;
  /*näkymän asiat*/
  if(oikeinko)
    ASETA_ASIAN_VARIT(syote,O_SYOTE1,O_SYOTE2);
  else {
    ASETA_ASIAN_VARIT(syote,V_SYOTE1,V_SYOTE2);
    strcpy(syotetxt, LISTALLA(&snsto,snsto_1*,kysymjarj[kysymind])->sana[!kysymind]);
  }
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

void kasittele_kysymys() {
  LAITA(kysymys);
  if( kysymind == kysymjarjpit ) {
    *kysymtxt = '\0';
    return;
  } else if( kysymind > kysymjarjpit ) {
    kysymind = 0;
    luo_uusi_jarjestys(&kysymjarj,&kysymjarjpit);
  }
  if(kysymind >= kysymjarjpit)
    return;
  char* kopioitava = LISTALLA(&snsto,snsto_1*,kysymjarj[kysymind])->sana[kumpi_kysym];
  int i;
  for(i=0; (i<maxpit_syote-1) && kopioitava[i]; i++)
    kysymtxt[i] = kopioitava[i];
  kysymtxt[i] = '\0';
  return;
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
  /*luetaan komennot komentoriviltä*/
  for(int i=1; i<argc; i++) {
    int n;
    sprintf(globchar2, " %s%n", argv[i],&n);
    globchar2 += n;
  }
  *globchar2 = '\0';
  
  for(int i=0; i<laitot_enum_pituus; i++)
    laitot |= 1<<i;
  while(SDL_PollEvent(&tapaht)); //tyhjennetään
  paivita_ikkunan_koko();
  if(strlen(globchar))
    kasittele_syote((Arg){.v=globchar});
  aja();

  puts("\033[1;33mVaroitus:\033[0m aja-funktio palasi, vaikka ei olisi pitänyt.");
  return 2;
}
