#include <stdlib.h>
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
  while(l->pit >= l->tilaa)
    l->taul = realloc(l->taul, (l->tilaa+=l->patka)*sizeof(char**));
}
