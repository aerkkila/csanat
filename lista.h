#ifndef __lista_h__
#define __lista_h__

#define LISTALLA(l,tyyppi,sij) ( (tyyppi)(l)->taul + sij )
#define LISTALLA_LOPUSTA(l,tyyppi,sij) ( (tyyppi)(l)->taul + (l)->pit+sij )
#define alusta_tama_lista(lista,patka,tyyppi) _alusta_tama_lista(lista,patka,sizeof(tyyppi))
#define alusta_lista(patka,tyyppi) _alusta_lista(patka,sizeof(tyyppi))
#define listastolla_arg( fun, listat, n, ... ) do	\
    for(int _i=0; _i<(n); _i++) {			\
      fun( (listat)+_i, __VA_ARGS__ );			\
    }							\
  while(0)
#define listastolla( fun, listat, n ) do	\
    for(int _i=0; _i<(n); _i++) {		\
      fun( (listat)+_i );			\
    }						\
  while(0)

typedef struct {
  int pit;
  int patka;
  int tilaa;
  int koko;
  int sij;
  void* taul;
} lista;

lista* _alusta_tama_lista(lista* l, unsigned patka, size_t koko);
lista* _alusta_lista(unsigned,size_t);
void jatka_listaa(lista*, int);
void* tuhoa_lista(lista*);
void tuhoa_tama_lista(lista*);
void tuhoa_tama_lista2(lista*);
void tuhoa_tama_lista2(lista*);
void listalle_kopioiden_mjon(lista* l, const char* restrict s);
#endif
