#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "lista.h"
#include "asetelma.h"
#include "menetelmiä.h"

int osattuja = 0;

int lue_tiedosto(const char* restrict nimi) {
  FILE* f = fopen(nimi, "r");
  if(!f)
    return -1;
  static int tiedostonro = -1;
  tiedostonro++;
  static int sanoja = -1; //luetusta tiedostosta otettujen sanojen tunnisteet ovat negatiivisia

  while (!feof(f)) {
    if (fgetc(f) == '\n' || feof(f))
      continue;
    fseek(f,-1,SEEK_CUR);
    if(!fgets(tmpc, maxpit_suote, f)) //sana luetaan
      break;
    if(*STRPAATE(tmpc) == '\n')
      *STRPAATE(tmpc) = 0;
    jatka_listaa(snsto, 1);
    for(int i=0; tmpc[i]; i++) //tämä hakee erottimen sanan ja käännöksen välillä ja sitten kopioi molemmat ja poistuu
      if((unsigned char)tmpc[i] < ' ') {
	tmpc[i] = 0;
	LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,-1)->sana = strdup(tmpc); //sana kirjoitetaan
	while((unsigned char)tmpc[++i] <= ' ');
	LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,-1)->kaan = strdup(tmpc+i); //käännös kirjoitetaan
	goto METAOSUUS;
      }
    /*jos for-silmukasta ei hypätty tämän ohi, erotin on rivinvaihto eli pitää lukea uusi rivi*/
    LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,-1)->sana = strdup(tmpc);
    if(!fgets(tmpc, maxpit_suote, f))
      TEE("Virhe: Ei luettu seuraavalta riviltä käännöstä sanalle %s", LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,1)->sana);
    if(*STRPAATE(tmpc) == '\n')
      *STRPAATE(tmpc) = 0;
    LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,-1)->kaan = strdup(tmpc);
  METAOSUUS:
#define A LISTALLA(snsto,snsto_s*,LISTA_LOPUSTA,-1)->meta
    A.id = sanoja--; //negatiivinen tunniste
    A.tiedostonro = tiedostonro;
    A.kierroksia = 0;
    A.osaamisia = 0;
    A.hetket = alusta_lista(2, uint32_t);
#undef A
  } //while not eof

  fclose(f);
  alussa = 1;
  listalle_kopioiden_mjon(tiedostot, nimi);
  return -sanoja-1;
}

void lopeta() {
  free(suoteol.teksti);
  TTF_CloseFont(suoteol.font);
  TTF_CloseFont(kauntiol.font);
  TTF_CloseFont(viestiol.font);
  TTF_CloseFont(tiedotol.font);
  if(viestiol.lista)
    viestiol.lista = tuhoa_lista(viestiol.lista);
  tiedotol.lista = tuhoa_lista2(tiedotol.lista);
  tiedostot = tuhoa_lista2(tiedostot);
  for(int i=0; i<kysynnat->pit; i++) {
    free(LISTALLA(kysynnat,kysynta_s*,LISTA_ALUSTA,i)->kysym);
    free(LISTALLA(kysynnat,kysynta_s*,LISTA_ALUSTA,i)->suote);
  }
  kysynnat = tuhoa_lista(kysynnat);
  for(int i=0; i<snsto->pit; i++) {
    tuhoa_lista(LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->meta.hetket);
    free(LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->sana);
    free(LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->kaan);
  }
  snsto = tuhoa_lista(snsto);
}

void tee_tiedot() {
  if(snsto->pit == 0) {
    for(int i=0; i<3; i++)
      LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,i)[0][0] = '\0';
    return;
  }
  /*sijainti n / m: m:ää ei päivitetä joka kerralla, vaan ainoastaan kierroksen alussa*/
  static int kierroksen_pituus = -1;
  if(kierroksen_pituus == -1)
    kierroksen_pituus = snsto->pit;

  int sijainti_kierroksella = 0; //vähennetään tulossa olevien osaamattomien määrä ja lisätään kierroksen pituus
  int i=0;
  osattuja = 0;
  
  /*nykyiseen asti ei muuteta sijaintia kierroksella*/
  for(; i<snsto->sij; i++)
    if( LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->meta.osaamisia >= osaamisraja ) //osattiin
      osattuja++;
  
  /*nykysijainnista eteenpäin vähennetään sijaintia_kierroksella*/
  for(; i<snsto->pit; i++) {
    if( LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,i)->meta.osaamisia < osaamisraja ) { //ei osattu
      sijainti_kierroksella--;
      continue;
    }
    osattuja++;
  }
  if(alussa) {
    kierroksen_pituus = snsto->pit - osattuja;
    alussa = 0;
  }
  sijainti_kierroksella += kierroksen_pituus;
  sprintf(*LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,0), "Sijainti: %i / %i", sijainti_kierroksella, kierroksen_pituus);
  sprintf(*LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,1), "Osattuja %i / %i", osattuja, snsto->pit);
  if(snsto->sij < snsto->pit)
    sprintf(*LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,2), "Tiedosto: \"%s\"",
	    *LISTALLA(tiedostot,char**,LISTA_ALUSTA,LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij)->meta.tiedostonro));
  else
    LISTALLA(tiedotol.lista,char**,LISTA_ALUSTA,2)[0][0] = '\0';
}

void edellinen_osatuksi() {
  extern int edellinen_sij;
  if(edellinen_sij < 0)
    return;
  sanameta_s* tiedot = &LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,edellinen_sij)->meta;
  uint32_t* osaaminen = LISTALLA(tiedot->hetket,uint32_t*,LISTA_LOPUSTA,-1);
  if( !(*osaaminen>>31) )
    tiedot->osaamisia += 1;
  *osaaminen |= 1<<31;
}

lista* pilko_sanoiksi(const char* restrict str) {
  const char* osoit = str;
  lista* r = alusta_lista(2,char**);
  while(sscanf(osoit, "%s", tmpc) == 1) {
    while((unsigned char)osoit[0] <= 0x20)
      osoit++; //välien yli ensin
    osoit += strlen(tmpc);
    /*välien yli*/
    jatka_listaa(r, 1);
    *LISTALLA(r,char**,LISTA_LOPUSTA,-1) = strdup(tmpc);
  }
  if(r->pit == 0) { //myös tyhjästä syötteestä tehdään lista
    jatka_listaa(r, 1);
    *LISTALLA(r,char**,LISTA_LOPUSTA,-1) = strdup("");
  }
  return r;
}

void sekoita() {
  srand((unsigned)time(NULL));
  for(int jaljella=snsto->pit-snsto->sij; jaljella>1; jaljella--) {
    int sij = rand() % jaljella;
    /*vaihdetaan sijainti ja viimeinen keskenään, viimeinen siirtyy aina lähemmäs*/
    snsto_s apu = *LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij+jaljella-1);
    *LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij+jaljella-1) = *LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij+sij);
    *LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij+sij) = apu;
  }
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
    tuhoa_lista2(viestiol.lista);
  viestiol.lista = alusta_lista(1,char**);
  jatka_listaa(viestiol.lista, 1);
  *LISTALLA(viestiol.lista,char**,LISTA_LOPUSTA,-1) = strdup(s);
  laita(viesti);
}

void uusi_kierros() {
  edellinen_sij = snsto->pit-1-osattuja;
  snsto->sij = 0;
  sekoita();
  osaamaton();
  alussa = 1;
  jatka_listaa(kysynnat, 1);
  memset(LISTALLA(kysynnat,kysynta_s*,LISTA_LOPUSTA,-1), 0, kysynnat->koko);
}

void osaamaton() {
  while(snsto->sij < snsto->pit) {
    snsto_s* sns = LISTALLA(snsto,snsto_s*,LISTA_ALUSTA,snsto->sij);
    if( sns->meta.osaamisia < osaamisraja) {
      kysymysol.teksti = sns->sana;
      return;
    }
    snsto->sij++;
  }
  kysymysol.teksti = NULL;
}
