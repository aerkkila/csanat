#include <strlista.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    
    metatied* m = malloc(sizeof(metatied));
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
  sana = _ylsvlms(sana, l); //listat ovat alussa
  kaan = _ylsvlms(kaan, lue_tiedosto(nimi));
  meta = _ylsvlms(meta, lue_tiedosto(nimi));
  return 0;
}
