#include <stdlib.h>
#include <string.h>
#include "lista.h"

void tuhoa_tama_lista(lista* l) {
  free(l->taul);
  l->taul = NULL;
  l->pit = 0;
  l->tilaa = 0;
}

void tuhoa_tama_lista2(lista* l) {
  for(int i=0; i<l->pit; i++)
    free(*LISTALLA(l,void**,i));
  tuhoa_tama_lista(l);
}

lista* _alusta_tama_lista(lista* l, unsigned patka, size_t koko) {
  l->patka = patka;
  l->tilaa = 0;
  l->koko = koko;
  l->pit = 0;
  l->taul = NULL;
  return l;
}

lista* _alusta_lista(unsigned patka, size_t koko) {
  lista* l = calloc(1, sizeof(lista));
  return _alusta_tama_lista(l,patka,koko);
}

void* jatka_listaa(lista* l, int n) {
  l->pit += n;
  int uusi_patkia = l->pit / l->patka + !!(l->pit % l->patka);
  if(uusi_patkia != l->tilaa / l->patka) {
    l->tilaa = uusi_patkia*l->patka;
    l->taul = realloc(l->taul, l->tilaa*l->koko);
  }
  return l->taul + (l->pit-1)*l->koko;
}

void listalle_kopioiden_mjon(lista* l, const char* restrict s) {
  jatka_listaa(l, 1);
  *LISTALLA_LOPUSTA(l,char**,-1) = strdup(s);
}
