#ifndef __lista_h__
#define __lista_h__

#define LISTA_LOPUSTA 1
#define LISTA_ALUSTA 0
#define LISTALLA(l,tyyppi,lopustako,sij) ( (tyyppi)(l)->taul + (l)->pit*lopustako+sij )
#define alusta_lista(patka,tyyppi) _alusta_lista(patka,sizeof(tyyppi))

typedef struct {
  int pit;
  int patka;
  int tilaa;
  int koko;
  int sij;
  void* taul;
} lista;

void jatka_listaa(lista*, int);
lista* _alusta_lista(unsigned,size_t);
void* tuhoa_lista(lista*);
void* tuhoa_lista2(lista*);
void listalle_kopioiden_mjon(lista* l, const char* restrict s);
#endif
