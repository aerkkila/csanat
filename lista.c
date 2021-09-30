#include <stdlib.h>
#include <string.h>
#include "lista.h"

void* tuhoa_lista(lista* l) {
  for(unsigned i=0; i<l->pit; i++)
    free(l->taul[i]);
  free(l->taul);
  free(l);
  return NULL;
}

void* tuhoa_clista(clista* l) {
  free(l->taul);
  free(l);
  return NULL;
}

lista* alusta_lista(unsigned patka) {
  lista* l = calloc(1, sizeof(lista));
  l->patka = patka;
  l->taul = calloc(patka, sizeof(char**));
  l->tilaa = patka;
  l->koko = sizeof(char**);
  return l;
}

clista* alusta_clista(unsigned patka) {
  clista* l = calloc(1, sizeof(clista));
  l->patka = patka;
  l->taul = calloc(patka, 1);
  l->tilaa = patka;
  l->koko = 1;
  return l;
}

void jatka_listaa(void* lv, int n) {
  lista* l = lv;
  l->pit += n;
  int uusi_patkia = l->pit / l->patka + 1;
  if(uusi_patkia != l->tilaa / l->patka) {
    l->tilaa = uusi_patkia*l->patka;
    l->taul = realloc(l->taul, l->tilaa*l->koko);
  }
}

void listalle_kopioiden(lista* l, const char* restrict s) {
  jatka_listaa(l, 1);
  l->taul[l->pit-1] = strdup(s);
}
