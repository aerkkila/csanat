#ifndef __lista_h__
#define __lista_h__

#define listan_loppu(l) ((l)->taul+(l)->pit-1)
#define CLISTA_LOPUSTA 1
#define CLISTA_ALUSTA 0
#define CLISTALTA(l,tyyppi,lopustako,sij) ( (tyyppi)(l)->taul + (l)->pit*lopustako+sij )

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
clista* alusta_clista(unsigned,int);
void* tuhoa_lista(void*);
void* tuhoa_clista(void*);
void listalle_kopioiden(lista* l, const char* restrict s);
#endif
