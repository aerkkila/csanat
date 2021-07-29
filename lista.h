#ifndef __lista_h__
#define __lista_h__

#define listan_loppu(l) ((l)->taul+(l)->pit-1)

typedef struct {
  unsigned pit;
  unsigned patka;
  unsigned tilaa;
  char** taul;
} lista;

void jatka_listaa(lista*, int);
lista* alusta_lista(unsigned);
void* tuhoa_lista(lista*);
#endif
