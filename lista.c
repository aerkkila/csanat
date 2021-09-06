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

lista* alusta_lista(unsigned patka) {
  lista* l = calloc(1, sizeof(lista));
  l->patka = patka;
  l->taul = calloc(patka, sizeof(char**));
  l->tilaa = patka;
  return l;
}

void jatka_listaa(lista* l, int n) {
  l->pit += n;
  int uusi_patkia = l->pit / l->patka + 1;
  if(uusi_patkia != l->tilaa / l->patka) {
    l->tilaa = uusi_patkia*l->patka;
    l->taul = realloc(l->taul, l->tilaa*sizeof(char**));
  }
}

void listalle_kopioiden(lista* l, const char* restrict s) {
  jatka_listaa(l, 1);
  l->taul[l->pit-1] = strdup(s);
}
