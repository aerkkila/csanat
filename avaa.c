#include <listat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "asetelma.h"

#define STRPAATE(a) ((a)[strlen(a)-1])

void* lue_tiedosto(const char* nimi) {
  static int lasku = -1; //kutsutaan kolmesti palauttaen aina yhden listan
  strlista *sana = NULL;
  static strlista *kaan = NULL;
  static ylista *meta = NULL;
  void* apu;
  static int tiedostonro = 0;

  lasku = (lasku+1) % 3;
  if(lasku == 1) {
    apu = _yalkuun(kaan);
    kaan = NULL;
    return apu;
  }
  if(lasku == 2) {
    apu = _yalkuun(meta);
    meta = NULL;
    return apu;
  }
  
  FILE *f = fopen(nimi, "r");
  if (!f) {
    lasku = -1;
    return NULL;
  }
  int sanoja = 0;
  char* luenta = malloc(maxpit_suote);
  
  while (!feof(f)) {
    //rivinvaihto lopussa ja tyhjät rivit välissä ohitetaan
    if (fgetc(f) == '\n' || feof(f))
      continue;
    fseek(f,-1,SEEK_CUR);
    fgets(luenta, maxpit_suote, f); //sana
    STRPAATE(luenta) = 0; //rivinvaihto pois lopusta
    sana = _strlisaa_kopioiden(sana, luenta);
    
    fgets(luenta, maxpit_suote, f); //käännös
    if(STRPAATE(luenta) == '\n')
      STRPAATE(luenta) = 0;
    kaan = _strlisaa_kopioiden(kaan, luenta);
    
    metatied* m = calloc(1,sizeof(metatied));
    m->parinro = sanoja++;
    m->tiednro = tiedostonro;
    meta = _ylisaa(meta, m);
  }
  free(luenta);
  fclose(f);
  return _yalkuun(sana);
}

int avaa(const char* nimi) {
  strlista* l = lue_tiedosto(nimi);
  if(!l)
    return 1;
  sana = _ylsvlms(sana, l); //l on alussa
  kaan = _ylsvlms(kaan, lue_tiedosto(nimi));
  meta = _ylsvlms(meta, lue_tiedosto(nimi));
  return 0;
}

int* randperm(int alku, int loppu, int* julos) {
  /*Menetelmä:
    lista luvuista [alku:loppu[
    for i in pituus {
    arvotaan luku n ∈ [0:listan_pituus[
    sijoitetaan listalta n. luku taulukkoon sijalle i
    poistetaan n. luku listalta
    } */
  ilista *l = NULL;
  for(int i=alku; i<loppu; i++)
    l = _ilisaa(l, i);
  l = _yalkuun(l);
  
  int n;
  int pit = loppu-alku;
  int *perm;
  if(!julos)
    perm = malloc(pit*sizeof(int));
  else
    perm = julos;
  for(int i=0; i<pit; i++) {
    n = rand() % (pit-i);
    perm[i] = (*(ilista*)_ynouda(l,n)).i;
    l = _yrm(l, &n, 1);
  }
  return perm;
}

void sekoita() {
  srand(time(NULL));
  int pit = _ylaske(sana);
  int *jarj = randperm(0, pit, NULL);
  SKMARGS(_yjarjestapit, jarj, pit);
  free(jarj);
}
