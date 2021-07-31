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
  static int tiedostonro = 0;
  
  FILE *f = fopen(nimi, "r");
  if (!f)
    return -1;

  int sanoja = 0;
  
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
    *(int*)(snsto->taul[snsto->pit-1]+0) = sanoja++;
    *(int*)(snsto->taul[snsto->pit-1]+4) = tiedostonro;
    /*loput ovat alussa nollia*/
  }
  fclose(f);
  return sanoja;
}

#define VAIHDA(a, b) do {			\
    char* apu = a;				\
    a = b;					\
    b = apu;					\
  } while(0)

void sekoita() {
  srand((unsigned)time(NULL));
  for(int jaljella=snsto->pit/3; jaljella>1; jaljella--) {
    int sij = rand() % jaljella;
    /*vaihdetaan sijainti ja viimeinen keskenään
      kolme peräkkäistä osoitinta kuuluvat aina yhteen*/
    for(int i=0; i<3; i++)
      VAIHDA(snsto->taul[jaljella*3-3+i], snsto->taul[sij*3+i]);
  }
}

void tee_tiedot() {
  if(snsto->pit == 0) {
    for(int i=0; i<2; i++)
      tiedotol.lista->taul[i][0] = '\0';
    return;
  }
  /*sijainti n / m: m:ää ei päivitetä joka kerralla, vaan ainoastaan kierroksen alussa*/
  static int kierroksen_pituus = -1;
  if(kierroksen_pituus == -1)
    kierroksen_pituus = snsto->pit/3; //tätä ei voi alustaa suoraan

  extern int alussa;
  int osattuja = 0;
  int sijainti_kierroksella = kierroksen_pituus; //vähennetään tästä tulossa olevien osaamattomien määrä
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
    sijainti_kierroksella = 0;
    alussa = 0;
  }
  snsto->sij = sij0;
  sprintf(tiedotol.lista->taul[0], "Sijainti: %i / %i", sijainti_kierroksella, kierroksen_pituus);
  sprintf(tiedotol.lista->taul[1], "Osattuja %i / %i", osattuja, snsto->pit/3);
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
void edellinen_kohta(const char* restrict suote, int* id) {
  int jatka = 1;
  int pit = strlen(suote);
  while(jatka && pit > *id)
    jatka = (suote[pit- ++(*id)] & 0xc0) == 0x80;
}

void seuraava_kohta(const char* restrict suote, int* id) {
  int pit = strlen(suote);
  while(*id && (suote[pit- --(*id)] & 0x80));
}
