#ifndef __menetelmia_h__
#define __menetelmia_h__
#include "lista.h"

#define META_ID 0
#define META_TIEDOSTONRO 1
#define META_KIERROKSIA 2
#define META_OSAAMISIA 3
#define META_OSAAMISET 4
#define NYT_OLEVA(l) ((l)->taul+(l)->sij)
#define VIIMEINEN(l) ((l)->taul[(l)->pit-1])
#define TOISEKSI_VIIM (kysynnat->taul[kysynnat->pit-2])
#define METATIEDOT         ((int*)NYT_OLEVA(snsto)[2])
#define TIEDOSTO_NYT       ((int*)( NYT_OLEVA(snsto)[2] )+META_TIEDOSTONRO)
#define KIERROKSIA_SANALLA ((int*)( NYT_OLEVA(snsto)[2] )+META_KIERROKSIA)
#define OSAAMISKERRAT      ((int*)( NYT_OLEVA(snsto)[2] )+META_OSAAMISIA)
#define SANAN_OSAAMISET ((long long unsigned*)((int*)( NYT_OLEVA(snsto)[2] )+META_OSAAMISET))

#define FOR_LISTA(l) for((l)->sij=0; (l)->sij<(l)->pit; (l)->sij++)

#define aseta_vari(v) SDL_SetRenderDrawColor(rend, v.r, v.g, v.b, v.a)
#define laita(jotain) (laitot |= (1u << jotain ## _enum))

#ifndef __laitot_enum__
#define __laitot_enum__
enum laitot_enum {
  kysymys_enum,
  suote_enum,
  kaunti_enum,
  tiedot_enum,
  viesti_enum,
  laitot_enum_pituus //lukuarvo kertoo pituuden
};
#endif

int avaa_tiedosto(const char*);
lista* pilko_sanoiksi(const char* restrict str);
void tee_tiedot();
void sekoita();
void edellinen_osatuksi();
void viestiksi(const char* restrict s); //määritelty käyttöliittymä.c:ssä
int edellinen_kohta(const char* restrict suote, int* id);
int seuraava_kohta(const char* restrict suote, int* id);
int xsijainti(tekstiolio_s* o, int p);
void viestiksi(const char* restrict);
void uusi_kierros();
void osaamaton();
#endif
