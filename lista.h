#ifndef __lista_h__
#define __lista_h__

#define listan_loppu(l) ((l)->taul+(l)->pit-1)

typedef struct {
  int pit;
  int patka;
  int tilaa;
  int sij;
  char** taul;
} lista;

void jatka_listaa(lista*, int);
lista* alusta_lista(unsigned);
void* tuhoa_lista(lista*);
void listalle_kopioiden(lista* l, const char* restrict s);
#endif
