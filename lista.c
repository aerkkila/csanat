#include <stdlib.h>
#include <string.h>
#include "lista.h"

void* tuhoa_lista(lista* l) {
  free(l->taul);
  free(l);
  return NULL;
}

void* tuhoa_lista2(lista* l) {
  for(int i=0; i<l->pit; i++)
    free(*LISTALLA(l,void**,LISTA_ALUSTA,i));
  free(l->taul);
  free(l);
  return NULL;
}

lista* _alusta_lista(unsigned patka, size_t koko) {
  lista* l = calloc(1, sizeof(lista));
  l->patka = patka;
  l->tilaa = 0;
  l->koko = koko;
  l->pit = 0;
  return l;
}

void jatka_listaa(lista* l, int n) {
  l->pit += n;
  int uusi_patkia = l->pit / l->patka + !!(l->pit % l->patka);
  if(uusi_patkia != l->tilaa / l->patka) {
    l->tilaa = uusi_patkia*l->patka;
    l->taul = realloc(l->taul, l->tilaa*l->koko);
  }
}

void listalle_kopioiden_mjon(lista* l, const char* restrict s) {
  jatka_listaa(l, 1);
  *LISTALLA(l,char**,LISTA_LOPUSTA,-1) = strdup(s);
}
