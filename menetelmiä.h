#ifndef __menetelmia_h__
#define __menetelmia_h__
#include <stdint.h>
#include "lista.h"
#include "grafiikka.h"

extern char* tmpc;

#define META_ID 0
#define META_TIEDOSTONRO 1
#define META_KIERROKSIA 2
#define META_OSAAMISIA 3
#define META_OSAAMISET 4
#define NYT_OLEVA(l) ((l)->taul+(l)->sij)
#define VIIMEINEN(l) ((l)->taul[(l)->pit-1])
#define TOISEKSI_VIIM (kysynnat->taul[kysynnat->pit-2])
#define METATIEDOT         ((int*)NYT_OLEVA(snsto)[2])
#define ID_SANALLA         (METATIEDOT + META_ID)
#define TIEDOSTO_NYT       (METATIEDOT + META_TIEDOSTONRO)
#define KIERROKSIA_SANALLA (METATIEDOT + META_KIERROKSIA)
#define OSAAMISKERRAT      (METATIEDOT + META_OSAAMISIA)
#define SANAN_OSAAMISET ((uint64_t*)(METATIEDOT + META_OSAAMISET))

#define FOR_LISTA(l,i) for((l)->sij=0; (l)->sij<(l)->pit; (l)->sij+=i)
#define VAIHDA(a, b) do {		\
    char* apu = a;			\
    a = b;				\
    b = apu;				\
  } while(0)
#define TEE(...) do {		       	\
    sprintf(tmpc, __VA_ARGS__);		\
    puts(tmpc);				\
    viestiksi(tmpc);			\
  } while(0)
#define VIESTIKSI(...) do {		\
    sprintf(tmpc, __VA_ARGS__);		\
    viestiksi(tmpc);			\
  } while(0)

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
void tallenna();
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
