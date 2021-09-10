#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lista.h"
#include "asetelma.h"
#include "menetelmiä.h"

#define STRPAATE(a) ((a)[strlen(a)-1])
extern char* tmpc;

int avaa_tiedosto(const char* nimi) {
  static int tiedostonro = -1;
  tiedostonro++;
  
  FILE *f = fopen(nimi, "r");
  if (!f)
    return -1;

  static int sanoja = 0;
  
  while (!feof(f)) {
    //rivinvaihto lopussa ja tyhjät rivit välissä ohitetaan
    if (fgetc(f) == '\n' || feof(f))
      continue;
    jatka_listaa(snsto, 3);
    fseek(f,-1,SEEK_CUR);
    if(!fgets(tmpc, maxpit_suote, f)) //sana
      break;
    STRPAATE(tmpc) = 0; //rivinvaihto pois lopusta
    snsto->taul[snsto->pit-3] = strdup(tmpc);
    
    if(!fgets(tmpc, maxpit_suote, f)) //käännös
      fprintf(stderr, "Virhe: Ei luettu käännöstä\n");
    if(STRPAATE(tmpc) == '\n')
      STRPAATE(tmpc) = 0;
    snsto->taul[snsto->pit-2] = strdup(tmpc);

    /*3:nteen tulee:
      (int)parin numero; (int)tiedoston numero; (int)montako kierrosta; (int)osaamisten määrä;
      (uint64)bittimaski osaamisista, viimeisin kierros vähiten merkitsevänä*/
    snsto->taul[snsto->pit-1] = calloc(24, 1);
    *((int*)(snsto->taul[snsto->pit-1])+META_ID) = sanoja++;
    *((int*)(snsto->taul[snsto->pit-1])+META_TIEDOSTONRO) = tiedostonro;
    /*loput ovat alussa nollia*/
  }
  fclose(f);
  alussa = 1;
  listalle_kopioiden(tiedostot, nimi);
  return sanoja;
}

void sekoita() {
  srand((unsigned)time(NULL));
  for(int jaljella=(snsto->pit-snsto->sij)/3; jaljella>1; jaljella--) {
    int sij = rand() % jaljella;
    /*vaihdetaan sijainti ja viimeinen keskenään, viimeinen siirtyy aina lähemmäs,
      kolme peräkkäistä osoitinta kuuluvat aina yhteen*/
    for(int i=0; i<3; i++)
      VAIHDA(snsto->taul[snsto->sij+jaljella*3-3+i], snsto->taul[snsto->sij+sij*3+i]);
  }
}

void tee_tiedot() {
  if(snsto->pit == 0) {
    for(int i=0; i<3; i++)
      tiedotol.lista->taul[i][0] = '\0';
    return;
  }
  /*sijainti n / m: m:ää ei päivitetä joka kerralla, vaan ainoastaan kierroksen alussa*/
  static int kierroksen_pituus = -1;
  if(kierroksen_pituus == -1)
    kierroksen_pituus = snsto->pit/3; //tätä ei voi alustaa suoraan

  int osattuja = 0;
  int sijainti_kierroksella = snsto->pit/3; //vähennetään tästä tulossa olevien osaamattomien määrä
  int sij0 = snsto->sij;
  int i=0;
  
  /*nykyiseen asti ei muuteta sijaintia kierroksella*/
  for(; i<sij0; i+=3) {
    snsto->sij=i;
    if(*OSAAMISKERRAT >= osaamisraja) //osattiin
      osattuja++;
  }
  
  /*nykysijainnista eteenpäin vähennetään sijaintia_kierroksella*/
  for(; i<snsto->pit; i+=3) {
    snsto->sij = i;
    if(*OSAAMISKERRAT < osaamisraja) { //ei osattu
      sijainti_kierroksella--;
      continue;
    }
    osattuja++;
  }
  if(alussa) {
    kierroksen_pituus = snsto->pit/3-osattuja;
    alussa = 0;
  }
  snsto->sij = sij0;
  sprintf(tiedotol.lista->taul[0], "Sijainti: %i / %i", sijainti_kierroksella, kierroksen_pituus);
  sprintf(tiedotol.lista->taul[1], "Osattuja %i / %i", osattuja, snsto->pit/3);
  if(snsto->sij < snsto->pit)
    sprintf(tiedotol.lista->taul[2], "Tiedosto: \"%s\"", tiedostot->taul[*TIEDOSTO_NYT]);
  else
    tiedotol.lista->taul[2][0] = '\0';
}

void edellinen_osatuksi() {
  extern int edellinen_sij;
  if(edellinen_sij < 0)
    return;
  int sij0 = snsto->sij;
  snsto->sij = edellinen_sij;
  if(!(*SANAN_OSAAMISET & 0x01)) {
    (*OSAAMISKERRAT)++;
    *SANAN_OSAAMISET += 1;
  }
  TOISEKSI_VIIM[strlen(TOISEKSI_VIIM)+1] = 0x01;
  snsto->sij = sij0;
}

lista* pilko_sanoiksi(const char* restrict str) {
  const char* osoit = str;
  lista* r = alusta_lista(2);
  while(sscanf(osoit, "%s", tmpc) == 1) {
    while((unsigned char)osoit[0] <= 0x20)
      osoit++; //välien yli ensin
    osoit += strlen(tmpc);
    /*välien yli*/
    jatka_listaa(r, 1);
    VIIMEINEN(r) = strdup(tmpc);
  }
  if(r->pit == 0) { //myös tyhjästä syötteestä tehdään lista
    jatka_listaa(r, 1);
    VIIMEINEN(r) = strdup("");
  }
  return r;
}

/*näissä siirrytään eteen- tai taakespäin koko utf-8-merkin verran*/
int edellinen_kohta(const char* restrict suote, int* id) {
  int jatka = 1;
  int pit = strlen(suote);
  int r = 0;
  while(jatka && pit > *id) {
    r=1;
    jatka = (suote[pit- ++(*id)] & 0xc0) == 0x80;
  }
  return r;
}

int seuraava_kohta(const char* restrict suote, int* id) {
  int pit = strlen(suote);
  int r = 0;
  while(*id && (r=1) && ((suote[pit- --(*id)] & 0xc0) == 0x80));
  return r;
}

int xsijainti(tekstiolio_s* o, int p) {
  int lev;
  char c0 = o->teksti[p];
  o->teksti[p] = '\0';
  TTF_SizeUTF8(o->font, o->teksti, &lev, NULL);
  o->teksti[p] = c0;
  return lev + o->toteutuma.x;
}

void viestiksi(const char* restrict s) {
  extern unsigned laitot;
  if(viestiol.lista)
    tuhoa_lista(viestiol.lista);
  viestiol.lista = alusta_lista(1);
  viestiol.lista->taul[0] = strdup(s);
  viestiol.lista->pit = 1;
  laita(viesti);
}

void uusi_kierros() {
  edellinen_sij = snsto->pit-3;
  snsto->sij = 0;
  sekoita();
  osaamaton();
  alussa = 1;
  jatka_listaa(kysynnat, 2);
  kysynnat->taul[kysynnat->pit-2] = calloc(2,1);
  kysynnat->taul[kysynnat->pit-1] = calloc(2,1);
}

void osaamaton() {
  while(snsto->sij < snsto->pit) {
    if( *OSAAMISKERRAT < osaamisraja) {
      kysymysol.teksti = *NYT_OLEVA(snsto);
      return;
    }
    snsto->sij+=3;
  }
  kysymysol.teksti = NULL;
}
