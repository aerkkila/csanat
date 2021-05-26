#include <strlista.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asetelma.h"

#define STRPAATE(a) ((a)[strlen(a)-1])

void* lue_tiedosto(const char* nimi) {
  static int lasku = 0; //kutsutaan kolmesti palauttaen aina yhden listan
  static strlista *sana = NULL;
  static strlista *kaan = NULL;
  static ylista *meta = NULL;
  static int tiedostonro = 0;

  if(lasku == 1)
    return kaan;
  if(lasku++ == 2)
    return meta;
  lasku %= 3;
  
  FILE *f = fopen(nimi, "r");
  if (!f) {
    lasku = 0;
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
    meta = _ylsvlms(meta, m);
  }
  free(luenta);
  fclose(f);
  return _yalkuun(sana);
}

int avaa(const char* nimi) {
  strlista* l = lue_tiedosto(nimi);
  if(!l)
    return 1;
  kysyntaol.lista = _ylsvlms(kysyntaol.lista, l);
  kaan = _ylsvlms(kaan, lue_tiedosto(nimi));
  meta = _ylsvlms(meta, lue_tiedosto(nimi));
  return 0;
}
