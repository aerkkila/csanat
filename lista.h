#ifndef __lista_h__
#define __lista_h__

#define listan_loppu(l) ((l)->taul+(l)->pit-1)

typedef struct {
  int pit;
  int patka;
  int tilaa;
  int koko;
  int sij;
  char** taul;
} lista;

typedef struct {
  int pit;
  int patka;
  int tilaa;
  int koko;
  int sij;
  char* taul;
} clista;

void jatka_listaa(void*, int);
lista* alusta_lista(unsigned);
clista* alusta_clista(unsigned);
void* tuhoa_lista(lista*);
void* tuhoa_clista(clista*);
void listalle_kopioiden(lista* l, const char* restrict s);
#endif
